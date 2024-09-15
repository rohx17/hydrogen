#pragma once

#include <iostream>
#include <optional>
#include <utility>
#include <string>
#include <vector>

//creating list of enum class of Tokens we wanted to have
enum class TokenType{
    exit,
    int_lit,
    semi
 };

//forming a structure with the type of token and the string value of it
struct Token {
    TokenType type;
    std::optional<std::string> value;
};


class Tokenizer {
    public:
    inline explicit Tokenizer(std::string src)
            : m_src(std::move(src))
        {
        }

    inline std::vector<Token> tokenize() {

        std::vector<Token> tokens;
        std::string buf;

        while(peek().has_value()) {
            if(std::isalpha(peek().value())) {
                buf.push_back(consume());

                while(peek().has_value() && std::isalnum(peek().value())) {
                    buf.push_back(consume());
                }

                if(buf=="exit") {
                    tokens.push_back({.type=TokenType::exit});
                    buf.clear();
                    continue;
                }
                else {
                    std::cerr <<"You messed up!"<<std::endl;
                    exit(EXIT_FAILURE);
                }
            }

            else if(std::isdigit(peek().value())) {
                buf.push_back(consume());

                while(peek().has_value() && std::isdigit(peek().value())) {
                    buf.push_back(consume());
                }

                tokens.push_back({.type=TokenType::int_lit,.value=buf});
                buf.clear();
                continue;
            }

            else if(peek().value()==';') {
                consume();
                tokens.push_back({.type=TokenType::semi});
                continue;
            }

            else if(std::isspace(peek().value())) {
                consume();
                continue;
            }

            else {
                std::cerr <<"You messed up!"<<std::endl;
                exit(EXIT_FAILURE);
            }

        }
        m_index=0;
        return tokens;
    }

    private:
        //Telling compiler return value is important, and if caller ignores the return value then throw a warning
        [[nodiscard]] inline std::optional<char> peek(int ahead = 1) const
        {
            if(m_index + ahead > m_src.length()) {
                return {};
            }
            else {
                return m_src.at(m_index);//.at() coz bound striking not undefined behavior
            }
        }

        inline char consume() {
            return m_src.at(m_index++);
        }

        const std::string m_src;
        size_t m_index=0;

};