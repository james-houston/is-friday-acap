# isfriday API
This directory contains the code associated with the Go API that the ACAP calls. It starts up an HTTP server using the `gin` library and serves the endpoint `/isfriday` on port 8080.

The IP contained in `main.go` should map to the interface IP of your local machine. If the IP is changed to the loopback address `127.0.0.1` you will only be able to access the API from the machine itself.

## Running
in order to run this first make sure you have Go installed on your machine, as long as the go version is higher than `1.16` the `github.com/gin-gonic/gin` should be installed automatically when you run the program. 

Next update `localIP` in `main.go` to match your IP, or the IP of the machine you plan to run the API on, and run:

`go run main.go`