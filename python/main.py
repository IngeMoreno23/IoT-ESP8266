import paho.mqtt.client as paho
import time
import json

"""
We will define some variables to store the broker address and the topic we want to subscribe to.
"""

broker_ip: str = ""
broker_port: int = 1883

upstream_topic: str = "upstream"
downstream_topic: str = "downstream"


client = paho.Client("client-001")