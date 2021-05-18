#!/bin/bash
export PYTHONPATH=stage/lib:$PYTHONPATH
gdb --args python3 test.py
