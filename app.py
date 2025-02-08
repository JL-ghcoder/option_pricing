from fastapi import FastAPI, HTTPException, Body
from fastapi.middleware.cors import CORSMiddleware
from typing import List, Optional
import option_pricing

app = FastAPI(
    title="Option Pricing API",
    description="API for pricing various types of options using different methods",
    version="2.0.0"
)

# CORS配置
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.get("/")
async def root():
    """API health check endpoint"""
    return {
        "status": "healthy",
        "message": "Option Pricing API is running"
    }

@app.post("/price/barrier")
async def price_barrier_option(
    data: dict = Body(...)
):
    """Price a barrier option using Monte Carlo simulation"""
    try:
        price = option_pricing.MonteCarloBarrierOption.price(
            data.get("S0"),
            data.get("K"),
            data.get("B"),
            data.get("T"),
            data.get("r"),
            data.get("sigma"),
            data.get("div", 0.0),
            data.get("num_steps", 252),
            data.get("num_paths", 10000)
        )
        return {"price": price}
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.post("/price/finite-difference/put")
async def price_fd_put(
    data: dict = Body(...)
):
    """使用有限差分法计算欧式看跌期权价格"""
    try:
        price = option_pricing.FiniteDifferencePricer.price_put(
            data.get("S0"),
            data.get("K"),
            data.get("r"),
            data.get("sigma"),
            data.get("T"),
            data.get("div", 0.0),
            data.get("grid_size", 100)
        )
        return {"price": price}
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.post("/price/finite-difference/call")
async def price_fd_call(
    data: dict = Body(...)
):
    """使用有限差分法计算欧式看涨期权价格"""
    try:
        price = option_pricing.FiniteDifferencePricer.price_call(
            data.get("S0"),
            data.get("K"),
            data.get("r"),
            data.get("sigma"),
            data.get("T"),
            data.get("div", 0.0),
            data.get("grid_size", 100)
        )
        return {"price": price}
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.post("/analysis/sensitivity")
async def analyze_sensitivity(
    data: dict = Body(...)
):
    """Perform sensitivity analysis with respect to volatility"""
    try:
        results = option_pricing.SensitivityTest.analyze(
            data.get("S0"),
            data.get("K"),
            data.get("B"),
            data.get("T"),
            data.get("r"),
            data.get("div", 0.0),
            data.get("vol_start", 0.1),
            data.get("vol_end", 0.4),
            data.get("num_points", 5),
            data.get("num_paths", 10000)
        )
        return {
            "data": [
                {"volatility": vol, "price": price}
                for vol, price in results
            ]
        }
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.post("/analysis/probability")
async def calculate_probability(
    data: dict = Body(...)
):
    """Calculate the probability of achieving a specific payoff"""
    try:
        prob = option_pricing.PayoffProbability.calculate(
            data.get("S0"),
            data.get("K"),
            data.get("B"),
            data.get("T"),
            data.get("r"),
            data.get("sigma"),
            data.get("div", 0.0),
            data.get("threshold", 5.0),
            data.get("num_paths", 100000),
        )
        return {"probability": prob}
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.post("/generate/paths")
async def generate_paths(
    data: dict = Body(...)
):
    """Generate and save price paths for visualization"""
    try:
        option_pricing.PathGenerator.generate_paths(
            data.get("S0"),
            data.get("K"),
            data.get("B"),
            data.get("T"),
            data.get("r"),
            data.get("sigma"),
            data.get("div", 0.0),
            "paths_data.csv",
            data.get("num_paths", 5)
        )
        return {"message": "Price paths generated successfully"}
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.get("/defaults")
async def get_default_parameters():
    """Get default parameters for testing"""
    return {
        "base_params": {
            "S0": 100.0,
            "K": 100.0,
            "T": 1.0,
            "r": 0.05,
            "sigma": 0.2,
            "div": 0.0
        },
        "barrier_params": {
            "B": 120.0,
            "num_steps": 252,
            "num_paths": 100000
        },
        "finite_difference_params": {
            "grid_size": 100
        },
        "sensitivity_params": {
            "vol_start": 0.1,
            "vol_end": 0.4,
            "num_points": 5
        },
        "probability_params": {
            "threshold": 5.0
        }
    }