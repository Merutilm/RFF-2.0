//
// Created by Merutilm on 2025-07-08.
//

#pragma once

namespace merutilm::vkh {
    struct exception : std::exception {
        std::string message;
        explicit exception(const std::string & message) : message(message) {
        }

        [[nodiscard]] const char *what() const noexcept override {
            return message.data();
        };

    };
    struct exception_nullptr final : exception{
        explicit exception_nullptr(const std::string & message) : exception("[NULL] " + message){}
    };

    struct exception_init final : exception{
        explicit exception_init(const std::string & message) : exception("[INITIALIZE] " + message){}
    };

    struct exception_invalid_args final : exception{
        explicit exception_invalid_args(const std::string & message) : exception("[ARGUMENTS] " + message){}
    };


    struct exception_invalid_state final : exception{
        explicit exception_invalid_state(const std::string & message) : exception("[STATE] " + message){}
    };
}
