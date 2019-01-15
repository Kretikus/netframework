#include "db.h"
#include "mysql_backend.h"

#include <iostream>
#include <map>
#include <vector>

struct Impl
{
    MySQL mysql;
};

static Impl * instance() {
    static Impl impl;
    return &impl;
}

bool Database::connect(const std::string & host, const std::string & userName, const std::string & passwd)
{
    Impl* impl = instance();
    auto ret = impl->mysql.connect(host, userName, passwd, 3306);
    if (!ret.first) {
        std::cerr << ret.second << std::endl;
    }
    return ret.first;
}

bool Database::setSchema(const std::string &schema)
{
    Impl* impl = instance();
    if (!impl->mysql.isConnected()) return false;
    return impl->mysql.setSchema(schema);
}

bool Database::execute(const std::string &sql)
{
    Impl* impl = instance();
    if (!impl->mysql.isConnected()) return false;

    auto ret = impl->mysql.executeQuery(sql);
    if (!ret.first) {
        std::cerr << ret.second << std::endl;
    }
    return ret.first;
}

class PreparedStatement::PSImpl
{
public:
    ~PSImpl() {
        for(unsigned int i=0; i < bindData.size(); ++i) {
            if (bindings[i].buffer_type == MYSQL_TYPE_VAR_STRING ||
                bindings[i].buffer_type == MYSQL_TYPE_STRING) {
                delete [] bindData[i].str;
            }
        }
    }

    union BindData {
        short shortno;
        int intno;
        char* str;
    };

    MYSQL_STMT* stmt_ = nullptr;

    // binding data for the parameters
    my_bool paramIsNull = 0;
    my_bool paramError = 0;
    std::vector<BindData> paramBindData;
    std::vector<unsigned long> paramLength;
    std::vector<MYSQL_BIND> paramBindings;

    // binding data for the result set
    std::map<std::string, unsigned int> columnData;


    std::vector<BindData> bindData;
    std::vector<my_bool>  isNull;
    std::vector<my_bool>  error;
    std::vector<MYSQL_BIND> bindings;
    std::vector<unsigned long> length;

    bool fillColumnData()
    {
        if (!stmt_) return false;
        if (columnData.empty()) {
            MYSQL_RES * res = mysql_stmt_result_metadata(stmt_);
            if (res) {
                unsigned int fieldCount = mysql_stmt_field_count(stmt_);
                bindData.resize(fieldCount);
                isNull.resize(fieldCount);
                error.resize(fieldCount);
                bindings.resize(fieldCount);
                length.resize(fieldCount);

                unsigned int i = 0;
                MYSQL_FIELD * field = mysql_fetch_field(res);
                while(field) {
                    columnData[std::string(field->name)] = i;

                    bindings[i].buffer_type = field->type;
                    bindings[i].is_null= &isNull[i];
                    bindings[i].length= &length[i];
                    bindings[i].error= &error[i];

                    switch(field->type) {
                    case MYSQL_TYPE_DECIMAL:
                        break;
                    case MYSQL_TYPE_TINY:
                        break;
                    case MYSQL_TYPE_SHORT:
                        bindings[i].buffer= reinterpret_cast<char *>(&bindData[i].shortno);
                        break;
                    case MYSQL_TYPE_LONG:
                        bindings[i].buffer= reinterpret_cast<char *>(&bindData[i].intno);
                        break;
                    case MYSQL_TYPE_FLOAT:
                        break;
                    case MYSQL_TYPE_DOUBLE:
                        break;
                    case MYSQL_TYPE_NULL:
                        break;
                    case MYSQL_TYPE_TIMESTAMP:
                        break;
                    case MYSQL_TYPE_LONGLONG:
                        break;
                    case MYSQL_TYPE_INT24:
                        break;
                    case MYSQL_TYPE_DATE:
                        break;
                    case MYSQL_TYPE_TIME:
                        break;
                    case MYSQL_TYPE_DATETIME:
                        break;
                    case MYSQL_TYPE_YEAR:
                        break;
                    case MYSQL_TYPE_NEWDATE:
                        break;
                    case MYSQL_TYPE_BIT:
                        break;
                    case MYSQL_TYPE_TIMESTAMP2:
                        break;
                    case MYSQL_TYPE_DATETIME2:
                        break;
                    case MYSQL_TYPE_TIME2:
                        break;
                    case MYSQL_TYPE_JSON:
                        break;
                    case MYSQL_TYPE_NEWDECIMAL:
                        break;
                    case MYSQL_TYPE_ENUM:
                        break;
                    case MYSQL_TYPE_SET:
                        break;
                    case MYSQL_TYPE_TINY_BLOB:
                        break;
                    case MYSQL_TYPE_MEDIUM_BLOB:
                        break;
                    case MYSQL_TYPE_LONG_BLOB:
                        break;
                    case MYSQL_TYPE_BLOB:
                        break;
                    case MYSQL_TYPE_VARCHAR:
                    case MYSQL_TYPE_VAR_STRING:
                    case MYSQL_TYPE_STRING:
                        bindData[i].str = new char[field->length+1];
                        bindings[i].buffer= bindData[i].str;
                        bindings[i].buffer_length= field->length;
                        break;
                    case MYSQL_TYPE_GEOMETRY:
                        break;
                    }

                    ++i;
                    field = mysql_fetch_field(res);
                }

                if (mysql_stmt_bind_result(stmt_, bindings.data()) != 0) {
                    Impl* impl = instance();
                    std::cerr <<  impl->mysql.getErrorString() << std::endl;
                }
            }
        }
        return true;
    }
};

PreparedStatement::PreparedStatement()
    : impl_(new PSImpl())
{
    Impl* impl = instance();
    if (!impl->mysql.isConnected()) return;

    impl_->stmt_ = impl->mysql.createStatement();
}

PreparedStatement::~PreparedStatement()
{
    if (impl_->stmt_) {
        mysql_stmt_free_result(impl_->stmt_);
        mysql_stmt_close(impl_->stmt_);
    }
}

bool PreparedStatement::prepare(const std::string & query)
{
    if (!impl_->stmt_) return false;
    const std::size_t paramCount = static_cast<std::size_t>(std::count(query.begin(), query.end(), '?'));
    impl_->paramLength.resize(paramCount);
    impl_->paramBindings.resize(paramCount);
    impl_->paramBindData.resize(paramCount);
    const int ret = mysql_stmt_prepare(impl_->stmt_, query.c_str(), static_cast<unsigned long>(query.size()));
    if (ret != 0) {
        Impl* impl = instance();
        std::cerr << impl->mysql.getErrorString() << std::endl;
        return false;
    }
    return true;
}

bool PreparedStatement::execute()
{
    if (!impl_->stmt_) return false;

    if (!impl_->paramBindData.empty()) {
        my_bool bindRet = mysql_stmt_bind_param(impl_->stmt_, impl_->paramBindings.data());
        if (bindRet != 0) {
            Impl* impl = instance();
            std::cerr << impl->mysql.getErrorString() << std::endl;
            return false;
        }
    }

    const int retExecute = mysql_stmt_execute(impl_->stmt_);
    if (retExecute != 0) {
        Impl* impl = instance();
        std::cerr << impl->mysql.getErrorString() << std::endl;
        return false;
    }
    const int retStoreResult = mysql_stmt_store_result(impl_->stmt_);
    if (retStoreResult != 0) {
        Impl* impl = instance();
        std::cerr << impl->mysql.getErrorString() << std::endl;
        return false;
    }

    impl_->fillColumnData();

    return true;
}

bool PreparedStatement::bindValue(unsigned int param, int v)
{
    if (!impl_->stmt_) return false;
    if (param > impl_->paramBindings.size()) return false;

    MYSQL_BIND * bnd = &(impl_->paramBindings[param]);
    memset(bnd, 0, sizeof(MYSQL_BIND));

    impl_->paramBindings[param].buffer_type = MYSQL_TYPE_LONG;
    impl_->paramBindings[param].is_null= &impl_->paramIsNull;
    impl_->paramLength[param] = sizeof(int);
    impl_->paramBindings[param].length= &impl_->paramLength[param];
    impl_->paramBindings[param].error= &impl_->paramError;
    impl_->paramBindData[param].intno = v;
    impl_->paramBindings[param].buffer= &impl_->paramBindData[param].intno;

    return true;
}

bool PreparedStatement::bindValue(unsigned int param, const std::string &v)
{
    if (!impl_->stmt_) return false;
    if (param > impl_->paramBindings.size()) return false;

    MYSQL_BIND * bnd = &(impl_->paramBindings[param]);
    memset(bnd, 0, sizeof(MYSQL_BIND));

    impl_->paramBindData[param].str = _strdup(v.c_str());

    impl_->paramBindings[param].buffer_type = MYSQL_TYPE_STRING;
    impl_->paramBindings[param].buffer  = impl_->paramBindData[param].str;
    impl_->paramBindings[param].buffer_length = static_cast<unsigned long>(v.size());
    impl_->paramBindings[param].length = nullptr;
    impl_->paramBindings[param].is_null = nullptr;

    return true;
}

bool PreparedStatement::nextResult()
{
    if (!impl_->stmt_) return false;
    return mysql_stmt_fetch(impl_->stmt_) == 0;
}

bool PreparedStatement::value(const std::string & colname, int &v)
{
  if (!impl_->stmt_) return false;

  auto it = impl_->columnData.find(colname);
  if (it == impl_->columnData.end()) return false;

  v = impl_->bindData[it->second].intno;

  return true;
}

bool PreparedStatement::value(const std::string &colname, std::string &v)
{
    if (!impl_->stmt_) return false;

    auto it = impl_->columnData.find(colname);
    if (it == impl_->columnData.end()) return false;

    v = impl_->bindData[it->second].str;

    return true;
}
