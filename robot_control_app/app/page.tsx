"use client"

import mqtt from "mqtt"
import { useEffect, useState } from "react"
import { ControlButtons } from "./components/ControlButtons"

export default function Home() {

  const [client, setClient] = useState<mqtt.MqttClient | null>(null)

  useEffect(() => {
    let mqttc: mqtt.MqttClient | null = null

    const connectMQTT = async () => {
      mqttc = await mqtt.connectAsync("mqtt://192.168.1.50", {
        port: 9001,
        username: "prod",
        password: "Traktor"
      })

      setClient(mqttc)
    }

    connectMQTT()

    return () => {
      if (mqttc) {
        mqttc.end();
      }
    };
  }, [])

  useEffect(() => {
    if (client != null) {
      client.on("connect", () => {
        console.log("MQTT Connected")
      })
    }
  }, [client])

  return (
    <main className="flex min-h-screen bg-cover bg-center bg-[url('/plant_bg.jpg')] flex-col items-center justify-between p-24">
      <ControlButtons broker={client} />
    </main>
  );
}
