#ifndef GUARD_CIRCUIT_SOURCE_HPP
#define GUARD_CIRCUIT_SOURCE_HPP
#include <cmath>
class Circuit::Source : public Circuit::Component
{
protected:
	double DC = 0;
	double smallSignalAmp = 0;

	double SINE_DC_offset = 0;
	double SINE_amplitude = 0;
	double SINE_frequency = 0;
	Source() = default;
	Source(const std::string &name, double DC, double smallSignalAmp, double SINE_DC_offset, double SINE_amplitude, double SINE_frequency, Schematic *schem) : Component(name, DC, schem)
	{

		this->name = name;
		this->DC = DC;
		this->smallSignalAmp = smallSignalAmp;
		this->SINE_DC_offset = SINE_DC_offset;
		this->SINE_amplitude = SINE_amplitude;
		this->SINE_frequency = SINE_frequency;
	}
	Source(const std::string &name, double value, const Circuit::Node *pos, const Circuit::Node *neg, Schematic *schem) : Source(name, value, 0, 0, 0, 0, schem)
	{
	}

public:
	double getSourceOutput(ParamTable *param, double t) const
	{
		return (DC + SINE_DC_offset + (SINE_amplitude)*std::sin(2.0 * M_PI * SINE_frequency * t));
	}
	bool isSource() const override
	{
		return true;
	}
	virtual bool isCurrent() const = 0;
};

class Circuit::Current : public Circuit::Source
{
	friend class Capacitor;

private:
	//Note to be used by
	Current(Schematic *schem)
	{
		this->DC = 0;
		this->schem = schem;
	}

public:
	Current(const std::string &name, double DC, const std::string &nodePos, const std::string &nodeNeg, double smallSignalAmp, double SINE_DC_offset, double SINE_amplitude, double SINE_frequency, Schematic *schem) : Source(name, DC, smallSignalAmp, SINE_DC_offset, SINE_amplitude, SINE_frequency, schem)
	{
		//NOTE for some reason swapped for current sources order - because of error in specification file
		schem->setupConnections2Node(this, nodeNeg, nodePos);
	}
	Current(const std::string &name, double DC, const std::string &nodePos, const std::string &nodeNeg, Schematic *schem) : Current(name, DC, nodePos, nodeNeg, 0, 0, 0, 0, schem)
	{
	}
	bool isCurrent() const override
	{
		return true;
	}
	double getCurrent(ParamTable *param, double t, double timestep = 0) const override
	{
		return getSourceOutput(param, t);
	}
};

class Circuit::Voltage : public Circuit::Source
{
	friend class Inductor;

private:
	Voltage(Schematic *schem)
	{
		DC = 0;
		this->schem = schem;
	}

public:
	Voltage(const std::string &name, double DC, const std::string &nodePos, const std::string &nodeNeg, double smallSignalAmp, double SINE_DC_offset, double SINE_amplitude, double SINE_frequency, Schematic *schem) : Source(name, DC, smallSignalAmp, SINE_DC_offset, SINE_amplitude, SINE_frequency, schem)
	{
		schem->setupConnections2Node(this, nodePos, nodeNeg);
	}
	Voltage(const std::string &name, double DC, const std::string &nodePos, const std::string &nodeNeg, Schematic *schem) : Voltage(name, DC, nodePos, nodeNeg, 0, 0, 0, 0, schem)
	{
	}
	bool isCurrent() const override
	{
		return false;
	}
	double getCurrent(ParamTable *param, double t, double timestep = 0) const override
	{
		double current = 0;
		double subCurrent = 0;
		Node *otherNode = nullptr;

		for (Component *comp : getPosNode()->comps)
		{
			if (comp != this)
			{
				subCurrent = abs(comp->getCurrent(param, t, timestep));
				for (Node *n : comp->nodes)
				{
					if (n != this->getPosNode())
					{
						otherNode = n;
						break;
					}
				}
				if (otherNode->voltage > getPosNode()->voltage)
				{
					current += subCurrent;
				}
				else
				{
					current -= subCurrent;
				}
			}
		}
		return current;
	}
};

#endif
