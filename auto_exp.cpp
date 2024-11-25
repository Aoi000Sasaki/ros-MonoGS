// g++ -std=c++17 auto_exp.cpp -o auto_exp -lyaml-cpp

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

int main() {
    std::string experiment_dir = "exp";
    std::string base_config_file = "/home/user/ws/src/ros-MonoGS/configs/use_db/kawaisann1.yaml";
    std::string experiment_config_file = "experiment_config.yaml";


    YAML::Node experiment_config = YAML::LoadFile(experiment_config_file);
    std::vector<std::map<std::string, std::string>> experiment_conditions;
    std::vector<int> experiment_iterations;

    for (const auto& condition : experiment_config["experiment_conditions"]) {
        std::map<std::string, std::string> condition_map;
        for (const auto& item : condition) {
            condition_map[item.first.as<std::string>()] = item.second.as<std::string>();
        }
        experiment_conditions.push_back(condition_map);
    }

    for (const auto& iteration : experiment_config["experiment_iterations"]) {
        experiment_iterations.push_back(iteration.as<int>());
    }

    int count = 0;
    while (fs::exists(experiment_dir + "/exp_" + std::to_string(count))) {
        count++;
    }
    experiment_dir = experiment_dir + "/exp_" + std::to_string(count);
    fs::create_directories(experiment_dir);

    YAML::Node base_config = YAML::LoadFile(base_config_file);

    for (size_t case_idx = 0; case_idx < experiment_conditions.size(); ++case_idx) {
        std::cout << "Starting case " << case_idx << "/" << (experiment_conditions.size() - 1) << std::endl;

        YAML::Node config = YAML::Clone(base_config);
        for (const auto& [key, value] : experiment_conditions[case_idx]) {
            config["use_db"][key] = value;
        }
        std::string save_dir = experiment_dir + "/case_" + std::to_string(case_idx);
        config["Results"]["save_dir"] = save_dir;

        fs::create_directories(save_dir);
        std::string config_file = save_dir + "/config_case" + std::to_string(case_idx) + ".yaml";
        std::ofstream config_out(config_file);
        config_out << config;
        config_out.close();

        std::string command = "python3 slam.py --config " + config_file;
        for (int iteration = 0; iteration < experiment_iterations[case_idx]; ++iteration) {
            std::cout << "Starting case " << case_idx << "/" << (experiment_conditions.size() - 1)
                      << " iteration " << iteration << "/" << (experiment_iterations[case_idx] - 1) << std::endl;

            int ret = system(command.c_str());
            if (ret != 0) {
                std::cerr << "Error in case " << case_idx << " iteration " << iteration << std::endl;
            }

            std::cout << "Finished case " << case_idx << "/" << (experiment_conditions.size() - 1)
                      << " iteration " << iteration << "/" << (experiment_iterations[case_idx] - 1) << std::endl;
        }
        std::cout << "Finished case " << case_idx << "/" << (experiment_conditions.size() - 1) << std::endl;
    }

    std::cout << "Finished" << std::endl;
    return 0;
}
