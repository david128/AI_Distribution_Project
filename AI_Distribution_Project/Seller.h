#pragma once
class Seller
{
	
private:
	
	//data about seller
	float stock;
	float maxStock;
	float salesPerDay;
	float travelTime;
public:

	Seller();
	Seller(float max, float spd, float travel);

	//getters and setters
	float GetStock() { return stock; }
	void SetStock(float s) { stock = s; }
	void AddStock(float s) { stock += s; }
	void SellStock(float s) { stock -= s; }
	float GetPercentageFull() ;
	float GetMaxStock() { return maxStock; }
	float GetSalesPerDay() { return salesPerDay; }
	float GetTravelTime() { return travelTime; }

};

