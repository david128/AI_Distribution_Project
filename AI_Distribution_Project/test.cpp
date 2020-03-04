//Code automatically generated with fuzzylite 6.0.

using namespace fl;

Engine* engine = new Engine;
engine->setName("distributionFuzzy1");
engine->setDescription("");

InputVariable* Stock = new InputVariable;
Stock->setName("Stock");
Stock->setDescription("");
Stock->setEnabled(true);
Stock->setRange(0.000, 100.000);
Stock->setLockValueInRange(false);
Stock->addTerm(new Triangle("Empty", -25.000, 0.000, 15.000));
Stock->addTerm(new Triangle("Low", 10.300, 25.600, 50.600));
Stock->addTerm(new Triangle("Medium", 30.000, 50.000, 70.000));
Stock->addTerm(new Triangle("WellStocked", 50.000, 75.000, 90.000));
Stock->addTerm(new Triangle("Full", 85.000, 100.000, 125.000));
engine->addInputVariable(Stock);

InputVariable* Distance = new InputVariable;
Distance->setName("Distance");
Distance->setDescription("");
Distance->setEnabled(true);
Distance->setRange(0.000, 6.000);
Distance->setLockValueInRange(false);
Distance->addTerm(new Triangle("Close", -3.000, 0.000, 3.000));
Distance->addTerm(new Triangle("MediumDistance", 1.500, 3.000, 4.500));
Distance->addTerm(new Triangle("Far", 3.000, 6.000, 9.000));
engine->addInputVariable(Distance);

InputVariable* SalesPerDay = new InputVariable;
SalesPerDay->setName("SalesPerDay");
SalesPerDay->setDescription("");
SalesPerDay->setEnabled(true);
SalesPerDay->setRange(0.000, 20.000);
SalesPerDay->setLockValueInRange(false);
SalesPerDay->addTerm(new Triangle("Low", -8.000, 0.000, 8.000));
SalesPerDay->addTerm(new Triangle("Moderate", 4.000, 10.000, 16.011));
SalesPerDay->addTerm(new Triangle("High", 12.000, 20.000, 28.000));
engine->addInputVariable(SalesPerDay);

InputVariable* DeliveryOnItsWay = new InputVariable;
DeliveryOnItsWay->setName("DeliveryOnItsWay");
DeliveryOnItsWay->setDescription("");
DeliveryOnItsWay->setEnabled(true);
DeliveryOnItsWay->setRange(0.000, 7.000);
DeliveryOnItsWay->setLockValueInRange(false);
DeliveryOnItsWay->addTerm(new Triangle("Soon", -2.500, 0.000, 2.500));
DeliveryOnItsWay->addTerm(new Triangle("OnItsWay", 1.500, 2.500, 5.000));
DeliveryOnItsWay->addTerm(new Triangle("NotSoon", 4.000, 5.000, 6.000));
DeliveryOnItsWay->addTerm(new Triangle("NoDelivery", 6.000, 7.000, 9.350));
engine->addInputVariable(DeliveryOnItsWay);

OutputVariable* output1 = new OutputVariable;
output1->setName("output1");
output1->setDescription("");
output1->setEnabled(true);
output1->setRange(0.000, 1.000);
output1->setLockValueInRange(false);
output1->setAggregation(new Maximum);
output1->setDefuzzifier(new Centroid(100));
output1->setDefaultValue(fl::nan);
output1->setLockPreviousValue(false);
output1->addTerm(new Triangle("VeryLow", -0.250, 0.000, 0.250));
output1->addTerm(new Triangle("Low", 0.000, 0.250, 0.500));
output1->addTerm(new Triangle("Average", 0.250, 0.500, 0.750));
output1->addTerm(new Triangle("High", 0.500, 0.750, 1.000));
output1->addTerm(new Triangle("VeryHigh", 0.750, 1.000, 1.250));
engine->addOutputVariable(output1);

RuleBlock* ruleBlock = new RuleBlock;
ruleBlock->setName("");
ruleBlock->setDescription("");
ruleBlock->setEnabled(true);
ruleBlock->setConjunction(new Minimum);
ruleBlock->setDisjunction(new Maximum);
ruleBlock->setImplication(new Minimum);
ruleBlock->setActivation(new General);
ruleBlock->addRule(Rule::parse("if Stock is Empty then output1 is VeryHigh", engine));
ruleBlock->addRule(Rule::parse("if Stock is Low then output1 is High", engine));
ruleBlock->addRule(Rule::parse("if Stock is Medium then output1 is Average", engine));
ruleBlock->addRule(Rule::parse("if Stock is WellStocked then output1 is Low", engine));
ruleBlock->addRule(Rule::parse("if Stock is Full then output1 is VeryLow", engine));
ruleBlock->addRule(Rule::parse("if Distance is Close then output1 is Low", engine));
ruleBlock->addRule(Rule::parse("if Distance is MediumDistance then output1 is Average", engine));
ruleBlock->addRule(Rule::parse("if Distance is Far then output1 is High", engine));
ruleBlock->addRule(Rule::parse("if SalesPerDay is Low then output1 is Low", engine));
ruleBlock->addRule(Rule::parse("if SalesPerDay is Moderate then output1 is Average", engine));
ruleBlock->addRule(Rule::parse("if SalesPerDay is High then output1 is High", engine));
ruleBlock->addRule(Rule::parse("if DeliveryOnItsWay is Soon then output1 is VeryLow", engine));
ruleBlock->addRule(Rule::parse("if DeliveryOnItsWay is OnItsWay then output1 is Low", engine));
ruleBlock->addRule(Rule::parse("if DeliveryOnItsWay is NotSoon then output1 is Average", engine));
ruleBlock->addRule(Rule::parse("if DeliveryOnItsWay is NoDelivery then output1 is High", engine));
engine->addRuleBlock(ruleBlock);


