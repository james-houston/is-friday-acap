# ACAP to check Friday API
This ACAP is built using the ACAP SDK version 4 for a camera with CPU arch `aarch64` running a minimum of AXIS OS of `10.9`. 

## Build for different architecture or OS
This may cause some incompatibilty, but it should anything over `10.7` _should_ be compatible.
- Use the table provided in [the ACAP SDK documentation](https://axiscommunications.github.io/acap-documentation/docs/develop/manifest-schemas/) to map your current Axis OS verison to the corresponding supported schema version and SDK version
- update the `schemaVersion` in `app/manifest.json` with the version in the `Schema` colum at the documentation webpage linked above
- in the `./Dockerfile` update the following lines:
    ```
    ARG ARCH=aarch64
    ARG VERSION=1.1
    ARG UBUNTU_VERSION=20.04
    ```

    - Change `ARCH` to match the architecture of your camera  
    - Change `VERSION` to match the SDK version that is supported by your Axis OS version, found in the table linked in step 1
    - Change `UBUNTU_VERSION` to the Ubuntu version used in the image that contains the SDK. In order to find the Ubuntu version, go to the [acap native sdk dockerhub releases page](https://hub.docker.com/r/axisecp/acap-native-sdk/tags) and scroll until you find the docker image that will be used. For example, the image that this ACAP is built with by default is found on page 5 as `1.1-aarch64-ubuntu20.04` where the `1.1` SDK version and `aarch64` is the arch

## Dependencies 
The Dockerfile compiles CURL and SSL for performing HTTP(S) calls to the API, as well as the open source cJSON library. Because cJSON is contained in a single source/header file, the source was directly added as a custom library to `./app/cjson`.

For the time being the Go API only uses HTTP, so SSL is unused. However the package is left available for when SSL is needed.

## Build the application

**Start by running the Go API included in `../go-api`**

Update `URL`  in `app/friday_event.c` source code with the IP the Go API is running at.

Standing in your working directory run the following commands:

`docker build --tag <APP_IMAGE> .`

where <APP_IMAGE> is the name to tag the image with. e.g. `friday_event`

Copy the result from the container image to a local directory build:

`docker cp $(docker create <APP_IMAGE>):/opt/app ./build`

Finally, install the ACAP through the camera Web UI.

## Development
A dev container setup is included in this directory as well. To learn more about how to use it for quicker development, you can follow the Axis docs [here](https://axiscommunications.github.io/acap-documentation/docs/develop/setting-up-visual-studio-code.html). One TODO is that the dev container does not have currently have CURL, SSL, or cJSON included. For developing a larger program this would be very helpful.