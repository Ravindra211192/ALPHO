#!/usr/bin/env python3
"""
Alphotronic - Sample Python Script for Raspberry Pi 5
This script demonstrates a basic loop and system information check.
"""

import sys
import os
import platform
import time

def print_pi_info():
    print("--- Alphotronic Pi 5 System Info ---")
    print(f"OS: {platform.system()} {platform.release()}")
    print(f"Python Version: {sys.version}")
    print(f"Hostname: {platform.node()}")
    print("------------------------------------")

def main():
    print_pi_info()
    
    counter = 0
    try:
        while counter < 5:
            print(f"Running... iteration {counter + 1}")
            counter += 1
            time.sleep(1)
        
        print("Done! If you can see this, Geany is working perfectly.")
        
    except KeyboardInterrupt:
        print("\nScript stopped by user.")

if __name__ == "__main__":
    main()
