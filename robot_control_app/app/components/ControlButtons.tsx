"use client"

import mqtt from 'mqtt'
import React from 'react'

type props = {
    broker: mqtt.MqttClient | null
}

export const ControlButtons = ({broker}: props) => {
    const MQTT_PUBLISH_TOPIC = 'cmnd/car/control'

    const handlePublish = (event: any, broker: mqtt.MqttClient | null) => {
        if (broker?.connected) {
            console.log(`Sending ${event.target.value}`)
            broker.publish(MQTT_PUBLISH_TOPIC, event.target.value)
        }
    }

    return (
        <div className="flex items-stretc my-auto w-screen px-8 gap-8 h-[40vh]">
            <button onClick={(e) => handlePublish(e, broker)} value={"left"} className="flex justify-center items-center bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded grow">
                <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={3} stroke="currentColor" className="w-12 h-12">
                    <path strokeLinecap="round" strokeLinejoin="round" d="M10.5 19.5 3 12m0 0 7.5-7.5M3 12h18" />
                </svg>
            </button>
            <button onClick={(e) => handlePublish(e, broker)} value={"forward"} className="flex justify-center items-center bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded grow">
                <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={3} stroke="currentColor" className="w-12 h-12">
                    <path strokeLinecap="round" strokeLinejoin="round" d="M4.5 10.5 12 3m0 0 7.5 7.5M12 3v18" />
                </svg>
            </button>
            <button onClick={(e) => handlePublish(e, broker)} value={"right"} className="flex justify-center items-center  bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded grow">
                <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={3} stroke="currentColor" className="w-12 h-12">
                    <path strokeLinecap="round" strokeLinejoin="round" d="M13.5 4.5 21 12m0 0-7.5 7.5M21 12H3" />
                </svg>
            </button>
        </div>
    )
}