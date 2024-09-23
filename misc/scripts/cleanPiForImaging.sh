#!/bin/bash
rm ~/.bash_history
rm ~/.xsession-errors*
rm ~/.ssh/*
sudo journalctl --rotate
sudo journalctl --vacuum-time=1s
