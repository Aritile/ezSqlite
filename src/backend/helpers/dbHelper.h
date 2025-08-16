/*
 * This file is part of ezSqlite.
 *
 * Copyright (C) 2025 Stephane Cuillerdier (Aka aiekick)
 *
 * ezSqlite is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ezSqlite is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with ezSqlite.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <variant>
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <ezlibs/ezClass.hpp>
#include <ezlibs/ezSingleton.hpp>

struct sqlite3;

// R�sultat g�n�rique de requ�te
struct ColumnInfo {
    std::string name;
    std::string declType;  // Type d�clar� dans la table (peut �tre vide)
};

struct Row {
    std::vector<std::variant<int64_t, double, std::string, std::vector<uint8_t>, std::nullptr_t>> values;
};

struct QueryResult {
    std::vector<ColumnInfo> columns;
    std::vector<Row> rows;
    bool isValid() const { return (!columns.empty()) && (!rows.empty()); }
    void clear() { *this = QueryResult(); }
};

struct SqliteDbDeleter final {
    void operator()(sqlite3* vDb) const noexcept;
};

class DBHelper final {
    IMPLEMENT_SINGLETON(DBHelper)
    DISABLE_CONSTRUCTORS(DBHelper)
    DISABLE_DESTRUCTORS(DBHelper)

private:  // (static)
    static const int32_t m_maxInsertAttempts;

private:  // (vars)
    std::unique_ptr<sqlite3, SqliteDbDeleter> m_sqliteDb{};
    std::string m_dataBaseFilePathName;
    bool m_transactionStarted{false};
    std::string m_lastErrorMsg{};

public:  // (methods)

    bool init(const std::string& vDBFilePathName) noexcept;
    void unit() noexcept;

    // DATABASE FILE
    bool isFileASqlite3DB(const std::string& vDBFilePathName) const noexcept;
    bool createDBFile(const std::string& vDBFilePathName) noexcept;
    bool openDBFile() noexcept;
    bool openDBFile(const std::string& vDBFilePathName) noexcept;
    void closeDBFile() noexcept;

    // TRANSACTIONS
    bool beginDBTransaction() noexcept;
    void commitDBTransaction() noexcept;
    void rollbackDBTransaction() noexcept;

    // MISC
    std::string getLastErrorMsg() const noexcept;

    // QUERY
    QueryResult executeQuery(const std::string& vSql) noexcept;

protected:  // (methods)

private:    // (methods)
    bool m_openDB() noexcept;
    void m_closeDB() noexcept;
    bool m_createDB() noexcept;
    bool m_enableForeignKey() noexcept;

    int32_t m_debugSqlite3Exec(          //
        const std::string& vDebugLabel,  //
        const std::string& vSqlQuery) noexcept;

    int32_t m_debugSqlite3PrepareV2(     //
        const std::string& vDebugLabel,  //
        const std::string& vSqlQuery,    //
        int32_t vNBytes,                 //
        struct sqlite3_stmt** vppStmt,   //
        const char** vpzTail) noexcept;
};
