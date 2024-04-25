#!/bin/bash

git add .
git commit -m "$1"
result=$(sudo cat /home/aris/.github_token)
git push https://"$result"@github.com/arigavits1/sockets-messenger.git main
