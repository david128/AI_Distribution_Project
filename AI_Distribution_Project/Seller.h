#pragma once
class Seller
{
	
private:
	

	float stock;
	float maxStock;
	float salesPerDay;
	float travelTime;
public:

	Seller();
	Seller(float max, float spd, float travel);

	float GetStock() { return stock; }
	float AddStock(float s) { stock += s; }
	float SellStock(float s) { stock -= s; }
	float GetMaxStock() { return maxStock; }
	float GetSalesPerDay() { return salesPerDay; }
	float GetTravelTime() { return travelTime; }

};

