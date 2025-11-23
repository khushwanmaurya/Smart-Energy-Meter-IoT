from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field
from fastapi.middleware.cors import CORSMiddleware
from typing import List, Optional
import uvicorn
from motor.motor_asyncio import AsyncIOMotorClient
import os
from datetime import datetime

app = FastAPI()

# Enable CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# MongoDB Connection
MONGO_URI = os.getenv("MONGO_URI", "mongodb://localhost:27017")
client = AsyncIOMotorClient(MONGO_URI)
db = client.energy_monitor
collection = db.readings

# Data Model
class EnergyData(BaseModel):
    vrms: float
    irms: float
    power: float
    relay_state: bool
    timestamp: datetime = Field(default_factory=datetime.utcnow)

class EnergyDataResponse(EnergyData):
    id: str = Field(alias="_id")

    class Config:
        populate_by_name = True
        json_encoders = {
            datetime: lambda v: v.isoformat()
        }

@app.post("/data")
async def receive_data(data: EnergyData):
    document = data.dict()
    await collection.insert_one(document)
    return {"status": "success", "message": "Data received"}

@app.get("/data", response_model=Optional[EnergyData])
async def get_data():
    # Get the latest document
    document = await collection.find_one(sort=[("_id", -1)])
    if document:
        # Remove _id for simple response or convert it
        return EnergyData(**document)
    return None

@app.get("/history", response_model=List[EnergyData])
async def get_history():
    cursor = collection.find().sort("_id", -1).limit(100)
    readings = await cursor.to_list(length=100)
    return [EnergyData(**doc) for doc in readings]

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)
