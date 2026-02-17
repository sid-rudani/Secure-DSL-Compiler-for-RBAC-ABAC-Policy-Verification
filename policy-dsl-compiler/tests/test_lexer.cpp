#include <gtest/gtest.h>
#include "lexer/Lexer.h"

TEST(LexerTest, BasicTokens) {
    std::string source = "policy test { user alice; }";
    Lexer lexer(source, "test.pol");
    auto tokens = lexer.tokenize();
    
    ASSERT_EQ(tokens.size(), 7); // policy, test, {, user, alice, }, EOF
    EXPECT_EQ(tokens[0].getType(), TokenType::POLICY);
    EXPECT_EQ(tokens[1].getType(), TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].getLexeme(), "test");
}

TEST(LexerTest, Comments) {
    std::string source = R"(
        // This is a comment
        user alice;
        /* Multi-line
           comment */
        user bob;
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    // Should only have: user, alice, ;, user, bob, ;, EOF
    EXPECT_EQ(tokens.size(), 7);
}

TEST(LexerTest, Operators) {
    std::string source = "< == != <= >= AND OR NOT";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    EXPECT_EQ(tokens[0].getType(), TokenType::LESS_THAN);
    EXPECT_EQ(tokens[1].getType(), TokenType::EQUALS);
    EXPECT_EQ(tokens[2].getType(), TokenType::NOT_EQUALS);
    EXPECT_EQ(tokens[3].getType(), TokenType::LESS_EQUAL);
}

TEST(LexerTest, StringLiterals) {
    std::string source = R"("hello world")";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // string + EOF
    EXPECT_EQ(tokens[0].getType(), TokenType::STRING_LITERAL);
    EXPECT_EQ(tokens[0].getLexeme(), "hello world");
}