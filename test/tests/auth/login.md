# GET /api/auth/register Endpoint Testing Documentation

This document outlines the testing scenarios for the `register_user` endpoint in the `AuthController` class.
   
## Test Scenarios

1. **Valid Registration**
    - **Description:** Test the endpoint with valid user registration data.
    - **Inputs:** 
        - Username, email, password, and user type in the request body.
    - **Expected Output:**
        - HTTP status code: 201 (Created).
        - JSON response containing the ID of the newly registered user.
		```json
		{
			"id": "user_id_registered"
		}  
		```
        - Set-Cookie header with JWT token for authentication.

2. **Duplicate Username**
    - **Description:** Test the endpoint with a username that already exists in the database.
    - **Inputs:** 
        - Existing username in the request body.
    - **Expected Output:**
        - HTTP status code: 400 (Bad Request).
        - Error message indicating that the username is already in use.

3. **Duplicate Email**
    - **Description:** Test the endpoint with an email address that already exists in the database.
    - **Inputs:** 
        - Existing email address in the request body.
    - **Expected Output:**
        - HTTP status code: 400 (Bad Request).
        - Error message indicating that the email address is already in use.

4. **Register Admin User with Invalid Community Name**
    - **Description:** Test the endpoint to register an admin user with an invalid community name.
    - **Inputs:** 
        - Admin user data with an invalid community name.
    - **Expected Output:**
        - HTTP status code: 500 (Internal Server Error).
        - Error message indicating an internal server error.

5. **Register Neighbor User with Nonexistent Community Code**
    - **Description:** Test the endpoint to register a neighbor user with a nonexistent community code.
    - **Inputs:** 
        - Neighbor user data with a nonexistent community code.
    - **Expected Output:**
        - HTTP status code: 404 (Not Found).
        - Error message indicating that the community does not exist.
