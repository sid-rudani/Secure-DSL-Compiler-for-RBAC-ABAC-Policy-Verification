#include <gtest/gtest.h>
#include "lexer/Lexer.h"
#include "parser/Parser.h"

TEST(ParserTest, SimplePolicy) {
    std::string source = R"(
        policy test {
            user alice;
            role admin;
            resource data;
            action read;
        }
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    EXPECT_EQ(ast->name, "test");
    EXPECT_EQ(ast->declarations.size(), 4);
}

TEST(ParserTest, RoleInheritance) {
    std::string source = R"(
        policy test {
            role intern;
            role analyst < admin;
        }
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    ASSERT_EQ(ast->declarations.size(), 2);
    
    auto* role2 = dynamic_cast<RoleDecl*>(ast->declarations[1].get());
    ASSERT_NE(role2, nullptr);
    EXPECT_EQ(role2->identifier, "analyst");
    EXPECT_EQ(role2->parent, "admin");
}

TEST(ParserTest, Rule) {
    std::string source = R"(
        policy test {
            user alice;
            resource data;
            action read;
            
            rule allow_read {
                effect: allow
                subject: user alice
                action: read
                resource: data
            }
        }
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    ASSERT_EQ(ast->rules.size(), 1);
    
    auto* rule = dynamic_cast<Rule*>(ast->rules[0].get());
    ASSERT_NE(rule, nullptr);
    EXPECT_EQ(rule->name, "allow_read");
    EXPECT_EQ(rule->effect, Rule::Effect::ALLOW);
    EXPECT_EQ(rule->subject, "alice");
}