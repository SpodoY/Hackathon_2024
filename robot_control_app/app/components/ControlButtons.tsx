"use client"

import mqtt from 'mqtt'
import React from 'react'

type props = {
    broker: mqtt.MqttClient | null
}

export const ControlButtons = ({ broker }: props) => {
    const MQTT_PUBLISH_TOPIC = 'car/control'
    const MQTT_CAR_RESPONSE_TOPIC = 'car/response'

    const handlePublish = (event: any, broker: mqtt.MqttClient | null, val: string) => {
        if (broker?.connected) {
            console.log(`Sending ${val}`)
            broker.publish(MQTT_PUBLISH_TOPIC, val)
        }
    }

    return (
        <div className='flex gap-4 flex-col mb-8'>
            <div className="flex items-stretc my-auto w-screen px-8 gap-8 h-[40vh]">
                <button onClick={(e) => handlePublish(e, broker, "L")} className="flex justify-center items-center bg-[#7A918D] hover:bg-[#69807C] text-white font-bold py-2 px-4 rounded grow">
                    <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={5} stroke="currentColor" className="w-12 h-12">
                        <path strokeLinecap="round" strokeLinejoin="round" d="M10.5 19.5 3 12m0 0 7.5-7.5M3 12h18" />
                    </svg>
                </button>
                <button onClick={(e) => handlePublish(e, broker, "F")} className="flex justify-center items-center bg-[#99C2A2] hover:bg-[#88B191] text-white font-bold py-2 px-4 rounded grow">
                    <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={5} stroke="currentColor" className="w-12 h-12">
                        <path strokeLinecap="round" strokeLinejoin="round" d="M4.5 10.5 12 3m0 0 7.5 7.5M12 3v18" />
                    </svg>
                </button>
                <button onClick={(e) => handlePublish(e, broker, "R")} className="flex justify-center items-center  bg-[#A2D86B] hover:bg-[#91C75A] text-white font-bold py-2 px-4 rounded grow">
                    <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={5} stroke="currentColor" className="w-12 h-12">
                        <path strokeLinecap="round" strokeLinejoin="round" d="M13.5 4.5 21 12m0 0-7.5 7.5M21 12H3" />
                    </svg>
                </button>
                <button onClick={(e) => handlePublish(e, broker, "V")} className="flex justify-center items-center bg-[#7A918D] hover:bg-[#69807C] text-white font-bold py-2 px-4 rounded grow">
                    <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={1.5} stroke="currentColor" className="w-6 h-6">
                        <path strokeLinecap="round" strokeLinejoin="round" d="m4.5 19.5 15-15m0 0H8.25m11.25 0v11.25" />
                    </svg>

                </button>
                <button onClick={(e) => handlePublish(e, broker, "B")} className="flex justify-center items-center bg-[#7A918D] hover:bg-[#69807C] text-white font-bold py-2 px-4 rounded grow">
                    <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={1.5} stroke="currentColor" className="w-6 h-6">
                        <path strokeLinecap="round" strokeLinejoin="round" d="m19.5 19.5-15-15m0 0v11.25m0-11.25h11.25" />
                    </svg>

                </button>
                <button onClick={(e) => handlePublish(e, broker, "N")} className="flex justify-center items-center bg-[#7A918D] hover:bg-[#69807C] text-white font-bold py-2 px-4 rounded grow">
                    <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={1.5} stroke="currentColor" className="w-6 h-6">
                        <path strokeLinecap="round" strokeLinejoin="round" d="M19.5 13.5 12 21m0 0-7.5-7.5M12 21V3" />
                    </svg>
                </button>
            </div>
            <div className="flex items-stretc my-auto w-screen px-8 gap-8 h-[40vh]">
                <button onClick={(e) => handlePublish(e, broker, "R")} className="flex justify-center items-center bg-[#7A918D] hover:bg-[#69807C] text-white font-bold py-2 px-4 rounded grow">
                    <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={1.5} stroke="currentColor" className="w-6 h-6">
                        <path strokeLinecap="round" strokeLinejoin="round" d="M9 15 3 9m0 0 6-6M3 9h12a6 6 0 0 1 0 12h-3" />
                    </svg>

                </button>
            </div>
        </div>
    )
}