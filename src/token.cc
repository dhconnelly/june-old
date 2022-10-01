#include "token.h"

std::string to_string(TokenType typ) {
    switch (typ) {
        case TokenType::Bool: return "Bool";
    }
}
