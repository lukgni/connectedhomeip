#!/bin/bash

set -e

if [ $# -lt 4 ]; then
    echo "Illegal number of parameters. Please define Github parameters: repository name, action ID, access token and CHIP directory path"
    exit 1
fi

GITHUB_REPOSITORY=$1
GITHUB_ACTION_ID=$2
GITHUB_TOKEN=$3
CHIP_DIR=$4

source $CHIP_DIR/scripts/tests/mbed/common.sh

echo "Downloading artifact from $GITHUB_ACTION_ID action in $GITHUB_REPOSITORY"
download_artifacts_gh $GITHUB_REPOSITORY $GITHUB_ACTION_ID $GITHUB_TOKEN binaries

echo "Get test sets"
mapfile -t test_sets <$CHIP_DIR/src/test_driver/mbed-functional/test_set.in

targer_number=$(cat devices.json | jq length)

flash_image_to_device shell CY8CPROTO_062_4343W binaries
pytest -rAV --platforms=CY8CPROTO_062_4343W --network=$AP_SSID:$AP_PASSWORD --echo_server=$AP_GATEWAY:$ECHO_SERVER_PORT $CHIP_DIR/src/test_driver/mbed-functional/shell/test_misc.py