# ServiceController::get_services Endpoint Testing Documentation

This document outlines the testing scenarios for the `get_services` endpoint in the `ServiceController` class.

## Test Scenarios

1. **Valid Request**

   - **Description:** Test the endpoint with a valid request from an authenticated user.
   - **Inputs:**
     - Valid JWT token via cookies.
     - User ID in the request body.
     - Community ID associated with the user.
   - **Expected Output:**
     - HTTP status code: 200.
     - JSON response containing an array of services belonging to the user's community.

2. **Valid Request with Status Filter**

   - **Description:** Test the endpoint with a valid request including a status filter.
   - **Inputs:**
     - Valid JWT token via cookies.
     - User ID in the request body.
     - Community ID associated with the user.
     - Status parameter in the query string (`open` or `closed`).
   - **Expected Output:**
     - HTTP status code: 200.
     - JSON response containing an array of services belonging to the user's community with the specified status.

3. **Invalid Status Parameter**

   - **Description:** Test the endpoint with an invalid status parameter.
   - **Inputs:**
     - Valid JWT token via cookies.
     - User ID in the request body.
     - Community ID associated with the user.
     - Invalid status parameter in the query string.
   - **Expected Output:**
     - HTTP status code: 400.
     - Error message indicating that the status parameter is not a valid value.

4. **Unauthorized Access**

   - **Description:** Test the endpoint without providing a JWT token.
   - **Inputs:**
     - No JWT token provided.
     - User ID in the request body.
     - Community ID associated with the user.
   - **Expected Output:**
     - HTTP status code: 401.
     - Error message indicating unauthorized access.

5. **Invalid User ID**

   - **Description:** Test the endpoint with an invalid or missing user ID.
   - **Inputs:**
     - Valid JWT token via cookies.
     - Invalid or missing user ID in the request body.
     - Community ID associated with the user.
   - **Expected Output:**
     - HTTP status code: 400.
     - Error message indicating an invalid or missing user ID.

6. **Internal Server Error**
   - **Description:** Test the endpoint when an internal server error occurs during processing.
   - **Inputs:**
     - Valid JWT token via cookies.
     - Valid user ID in the request body.
     - Community ID associated with the user.
     - Trigger an internal server error within the endpoint.
   - **Expected Output:**
     - HTTP status code: 500.
     - Error message indicating an internal server error.


```json
{
  "id": "service_uuidv4",
  "creator_id": "creator_uuidv4",
  "title": "service_title",
  "description": "service_description",
  "price": "service_price",
  "status": "service_status  ('open' | 'closed')",
  "type": "service_type ('offered' | 'requested')",
  "image_url": "optional",
  "creator": {
    "id": "creator_uuidv4",
    "username": "username",
    "type": "('admin' | 'neighbor')",
    "email": "email@gmail.com",
    "balance": 1000,
    "created_at": "date",
    "updated_at": "date"
  },
  "bueyer_id": "optional",
  "buyer": {
    "id:": "buyer_uuidv4",
    "username": "username",
    "type": "('admin' | 'neighbor')",
    "email": "email@gmail.com",
    "balance": 1000,
    "created_at": "date",
    "updated_at": "date"
  }
}

```

- `image_url` is optional field
- `buyer_id` and `buyer` are optional fields


crear usuario y que tenga 3 servicios, 
testear que todo va bien ya que hay toekn valido,
el json de respuesta tiene "services" y no es vacio, se comprueba todas su propiedades 
