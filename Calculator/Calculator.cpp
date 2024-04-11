#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace httplib;
using namespace std;

using json = nlohmann::json;

struct CalculationRequest {
    double num1;
    double num2;
};

struct CalculationResponse {
    double result;
};

double add(double num1, double num2) {
    return num1 + num2;
}

double subtract(double num1, double num2) {
    return num1 - num2;
}

double multiply(double num1, double num2) {
    return num1 * num2;
}

std::pair<double, bool> divide(double num1, double num2) {
    if (num2 == 0) {
        return std::make_pair(0, false);
    }
    return std::make_pair(num1 / num2, true);
}

void calculate(const Request& req, Response& res) {
    CalculationRequest calc_req;
    json j_req = json::parse(req.body);
    calc_req.num1 = j_req["num1"];
    calc_req.num2 = j_req["num2"];

    CalculationResponse calc_res;
    if (req.path == "/add") {
        calc_res.result = add(calc_req.num1, calc_req.num2);
    }
    else if (req.path == "/subtract") {
        calc_res.result = subtract(calc_req.num1, calc_req.num2);
    }
    else if (req.path == "/multiply") {
        calc_res.result = multiply(calc_req.num1, calc_req.num2);
    }
    else if (req.path == "/divide") {
        auto result = divide(calc_req.num1, calc_req.num2);
        if (!result.second) {
            res.status = 400;
            return;
        }
        calc_res.result = result.first;
    }
    else {
        res.status = 404;
        return;
    }

    json j_res = { {"result", calc_res.result} };
    res.set_content(j_res.dump(), "application/json");
}

int main() {
    Server svr;

    svr.Post("/add", calculate);
    svr.Post("/subtract", calculate);
    svr.Post("/multiply", calculate);
    svr.Post("/divide", calculate);

    std::cout << "Listening on port 8080..." << std::endl;

    if (!svr.listen("localhost", 8080)) {
        std::cout << "Failed to start server." << std::endl;
    }

    return 0;
}