curl  -H "Content-Type: application/json" \
  -d '{"Image": "alpine", "Cmd": ["echo", "hello world"]}' \
  -X POST http://192.168.1.33:2376/containers/create
#{"Id":"a2d1012fd96862b842b5bf5210a4dedf4dfb88143f8c339323cec046564de9af","Warnings":null}

curl  -X POST http://192.168.1.33:2376/containers/a2d1012fd96862b842b5bf5210a4dedf4dfb88143f8c339323cec046564de9af/start

curl  -X POST http://192.168.1.33:2376/containers/a2d1012fd96862b842b5bf5210a4dedf4dfb88143f8c339323cec046564de9af/wait
#{"StatusCode":0}

curl  "http://192.168.1.33:2376/containers/a2d1012fd96862b842b5bf5210a4dedf4dfb88143f8c339323cec046564de9af/logs?stdout=1"
