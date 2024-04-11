import mqtt from 'mqtt'
import React, { useEffect, useState } from 'react'

type props = {
    broker: mqtt.MqttClient | null
}

type plantSettings = {
    light: string,
    moisture: string,
    temp: number
}

const PlantForm = ({ broker }: props) => {
    const MQTT_PUBLISH_TOPIC = 'plant/settings'
    const [plantSettings, setPlantSettings] = useState<plantSettings>({
        light: "LOW",
        moisture: "LOW",
        temp: 0
    })

    const handleSubmit = (event: any) => {
        event.preventDefault()
        console.log(plantSettings)
        if (broker?.connected) {
            broker.publish(MQTT_PUBLISH_TOPIC, JSON.stringify(plantSettings))
        }
    }

    useEffect(() => {
        broker?.publish(MQTT_PUBLISH_TOPIC, JSON.stringify(plantSettings))
    }, [broker])

    return (
        <form onSubmit={handleSubmit}>
            <div className='flex flex-col items-center mb-6 gap-3'>
                <div>
                    <label> Plant light level </label>
                    <select value={plantSettings.light} onChange={e => setPlantSettings({...plantSettings, light: e.target.value})} className='bg-white border border-gray-200 text-gray-700 py-3 px-4 pr-8 rounded leading-tight focus:outline-none focus:bg-white focus:border-gray-500' id="lightType" name="plantLight">
                        <option value="LOW">Low</option>
                        <option value="MEDIUM">Medium</option>
                        <option value="BRIGHT">High</option>
                    </select>
                </div>

                <div>
                    <label> Plant Moisture level </label>
                    <select value={plantSettings.moisture} onChange={(e) => setPlantSettings({...plantSettings, moisture: e.target.value})} className='bg-white border border-gray-200 text-gray-700 py-3 px-4 pr-8 rounded leading-tight focus:outline-none focus:bg-white focus:border-gray-500' id="moistureType" name="plantMoisture">
                        <option value="LOW">Low</option>
                        <option value="MEDIUM">Medium</option>
                        <option value="HIGH">High</option>
                    </select>
                </div>

                <div>
                    <label> Plant temperature °C </label>
                    <input min={0} max={50} onChange={(e) => setPlantSettings({...plantSettings, temp: Number(e.target.value)})} className='bg-white border border-gray-200 text-gray-700 py-3 px-4 pr-8 rounded leading-tight focus:outline-none focus:bg-white focus:border-gray-500' type="number" />
                </div>

                <button className='bg-white hover:bg-white-500 text-gray-700 font-semibold py-2 px-4 border border-gray-500 hover:border-transparent rounded' type='submit'> Save config </button>
            </div>
        </form>
    )
}

export default PlantForm