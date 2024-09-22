//ppl use antlr to create parsers
#pragma once
#include "./arena.h"
#include "tokenization.h"
#include <variant>


struct NodeExpr;

struct NodeExprIntLit {
    Token int_lit;
};

struct NodeExprIdent {
    Token ident;
};


struct BinExprMulti {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct BinExprAdd {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExpr {
    std::variant<BinExprAdd*, BinExprMulti*> var;
};

struct NodeExpr {
    std::variant<NodeExprIntLit*, NodeExprIdent*, NodeBinExpr*> var;
};







struct NodeStmtExit {
    NodeExpr* expr;
};


struct NodeStmtLet {
    Token ident;
    NodeExpr* expr;
};

struct NodeStmt {
    std::variant<NodeStmtExit*, NodeStmtLet*> var;
};



struct NodeProg {
    std::vector<NodeStmt*> stmts;
};





class Parser {
public:
    inline explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)),m_allocator(1024*1024*4)//4 megabytes
    {

    }

    std::optional<NodeExpr*> parse_expr() {
        if(peek().has_value() && peek().value().type == TokenType::int_lit) {
            auto expr_int_lit = m_allocator.alloc<NodeExprIntLit>();
            expr_int_lit->int_lit = consume();
            auto expr = m_allocator.alloc<NodeExpr>();
            expr->var = expr_int_lit;
            return expr;
        }
        else if(peek().has_value() && peek().value().type == TokenType::ident) {
            auto expr_ident = m_allocator.alloc<NodeExprIdent>();
            expr_ident->ident = consume();
            auto expr = m_allocator.alloc<NodeExpr>();
            expr->var = expr_ident;
            return expr;
        }
        else {
            return {};
        }
    }

    std::optional<NodeStmt*> parse_stmt() {
        if(peek().value().type==TokenType::exit && peek(1).has_value() && peek(1).value().type == TokenType::open_paren) {
            consume();
            consume();
            auto stmt_exit = m_allocator.alloc<NodeStmtExit>();

            if(auto node_expr = parse_expr()) {
                stmt_exit->expr=node_expr.value();
            }
            else {
                std::cerr<<"Invalid Expression"<<std::endl;
                exit(EXIT_FAILURE);
            }

            if(peek().has_value() && peek().value().type == TokenType::close_paren) {
                consume();
            }
            else {
                std::cerr<<"Expected ')' "<<std::endl;
                exit(EXIT_FAILURE);
            }
            if(peek().has_value()&&peek().value().type == TokenType::semi) {
                consume();
            }
            else {
                std::cerr<<"Expected ';' "<<std::endl;
                exit(EXIT_FAILURE);
            }
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_exit;
            return stmt;

        }

        else if (peek().has_value() && peek().value().type == TokenType::let && peek(1).has_value() && peek(1).value().type == TokenType::ident && peek(2).has_value() && peek(2).value().type == TokenType::eq) {
            consume();

            auto stmt_let = m_allocator.alloc<NodeStmtLet>();
            stmt_let->ident = consume();
            consume();
            if(auto expr = parse_expr()) {
                stmt_let->expr=expr.value();
            }
            else {
                std::cerr<<"Invalid Expression"<<std::endl;
                exit(EXIT_FAILURE);
            }
            if(peek().has_value() && peek().value().type == TokenType::semi) {
                consume();

            }
            else {
                std::cerr<<"Expected ';' "<<std::endl;
                exit(EXIT_FAILURE);
            }
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_let;
            return stmt;
        }
        else {
            return {};
        }


    }

    std::optional<NodeProg> parse_prog(){
        NodeProg prog;

        while(peek().has_value()) {
            if(auto stmt = parse_stmt()) {
                prog.stmts.push_back(stmt.value());
            }
            else {
                std::cerr<<"Invalid Expression"<<std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }

    private:
    //Telling compiler return value is important, and if caller ignores the return value then throw a warning
    [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const
    {
        if(m_index + offset >= m_tokens.size()) {
            return {};
        }
        else {
            return m_tokens.at(m_index+offset);//.at() coz bound striking not undefined behavior
        }
    }

    inline Token consume() {
        return m_tokens.at(m_index++);
    }

    size_t m_index=0;
    std::vector<Token> m_tokens;
    ArenaAllocator m_allocator;

};