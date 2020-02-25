#pragma once

struct Lorry
{
	float daysUntilReturn;
	float carryLimit;
	float carrying;
};

class DistributionCentre
{
private:
	float maxStock;
	float currentStock;

public:

	DistributionCentre();

	DistributionCentre(float max, float startingStock, Lorry l);

	Lorry lorries[2];

	void AddStock(float s) { currentStock += s; }
	void RemoveStock(float s) { currentStock -= s; }
	float GetCurrentStock() { return currentStock; }
	float GetMaxStock() { return maxStock; }
	



};

