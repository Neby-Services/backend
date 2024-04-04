# Integration Testing for C++ Server

## Table of Contents
1. [Project Description](#project-description)
2. [Tools Used](#tools-used)
3. [Project Objective](#project-objective)
4. [Project Structure](#project-structure)
5. [Running the Tests](#running-the-tests)
    - [Method 1: Automated Execution with Docker](#method-1-automated-execution-with-docker)
    - [Method 2: Manual Execution within the Container](#method-2-manual-execution-within-the-container)
6. [Tests Organization](#tests-organization)

## Project Description

This project focuses on performing integration tests for all endpoints of a server implemented in C++. Integration tests are a type of test that verify the interaction between various components of a system to ensure their proper functioning together.

## Tools Used

The project utilizes the following tools for conducting integration tests:

- **Google Test:** Google Test is a unit testing framework for C++. In this project, it's used to write and execute automated tests that verify the behavior of the server endpoints.

- **CPR:** CPR is a C++ library that facilitates HTTP requests. It's used in this project to send requests to the server endpoints during integration testing.

- **nlohmann/json:** nlohmann/json is a C++ library for JSON data handling. In this project, it's used to manipulate input and output data of the endpoints during integration testing.

## Project Objective

The primary objective of this project is to ensure that all server endpoints function correctly and integrate properly with other components of the system. This is achieved through the writing and execution of automated tests that cover different scenarios and use cases.

## Project Structure

The project is organized into different files and folders, including:

- **`tests` Directory:** Contains the source code files for integration tests written with Google Test.

- **`CMakeLists.txt` File:** This file is used to configure and generate the project's build system using CMake.

- **Other Source Code Files:** In addition to test files, the project may include source code files that implement the server endpoints and other components necessary for integration testing.

## Running the Tests

To execute the integration tests, there are two different ways to run the tests:

### Method 1: Automated Execution with Docker

1. Make sure you are in the DevOps repository.
2. Bring up all necessary containers by running the `test.yml` file. This file will start all containers required to run the integration tests.

   ```bash
   docker compose -f test.yml up --build
   ```

3. Once Docker has finished bringing everything up, all existing tests will be automatically executed.

### Method 2: Manual Execution within the Container

1. In the `devops` repository, execute the following command to acccess the test container:
   ```bash
   docker exec -it test bash
   ```
2. Within the container, you can run the tests you're interested in. For example, if your tests have the following structure `TEST(suite_name, test_name)`:
   - change to the test directory:
   ```bash
   cd test
   ```
   - run a specific test using the Google Test filter:
   ```bash
   ./test --gtest_filter="suite_name.test_name"
   ```
   - to run all test starting with a specific name, use the asterisk as a wildcard:
   ```bash
   ./test --gtest_filter="suite_name*"
   ```

These methods allow you to execute the integration tests automatically or manually, depending on your needs and preferences. The second method is useful when you only need to run specific tests within the project due to time constraints or other considerations.

## Tests Organization

### Auth
- ["GET /api/auth/register"](tests/auth/lgoin.md)

### Users

### Services 

### Notifications
