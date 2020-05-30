#ifndef GUARD_CIRCUIT_SIMULATOR_HPP
#define GUARD_CIRCUIT_SIMULATOR_HPP
#include <sstream>
class Circuit::Simulator
{
private:
    Schematic *schem;
    double tranStopTime;
    double tranSaveStart;
    double tranStepTime;

    std::stringstream spiceStream;
    std::stringstream csvStream;

    void spicePrintTitle()
    {
        spiceStream << "Time\t";
        for (auto node_pair : schem->nodes)
        {
            spiceStream << "V(" << node_pair.first << ")\t";
        }
        for (auto comp_pair : schem->comps)
        {
            spiceStream << "I(" << comp_pair.first << ")\t";
        }
        spiceStream << "\n";
    }
    void csvPrintTitle()
    {
        csvStream << "Time,";
        for (auto node_pair : schem->nodes)
        {
            csvStream << "V(" << node_pair.first << "),";
        }
        for (auto comp_pair : schem->comps)
        {
            csvStream << "I(" << comp_pair.first << "),";
        }
        csvStream << "\n";
    }
    void spicePrint(ParamTable *param, double time, double timestep)
    {
        spiceStream << time << "\t";
        for (auto node_pair : schem->nodes)
        {
            spiceStream << node_pair.second->voltage << "\t";
        }
        for (auto comp_pair : schem->comps)
        {
            spiceStream << comp_pair.second->current(param, time, timestep) << "\t";
        }
        spiceStream << "\n";
    }
    void csvPrint(ParamTable *param, double time, double timestep)
    {
        csvStream << time << ",";
        for (auto node_pair : schem->nodes)
        {
            csvStream << node_pair.second->voltage << ",";
        }
        for (auto comp_pair : schem->comps)
        {
            csvStream << comp_pair.second->current(param, time, timestep) << ",";
        }
        csvStream << "\n";
    }

public:
    enum SimulationType
    {
        OP,
        TRAN,
        DC,
        SMALL_SIGNAL
    };

    const SimulationType type;

    Simulator(Schematic *schem, SimulationType type) : type(type), schem(schem) {}
    Simulator(Schematic *schem, SimulationType type, double tranStopTime, double tranSaveStart, double tranStepTime) : Simulator(schem, type)
    {

        this->tranStopTime = tranStopTime;
        this->tranSaveStart = tranSaveStart;
        this->tranStepTime = tranStepTime;
    }
    void run()
    {
        spiceStream.str("");
        csvStream.str("");
        for (ParamTable *param : schem->tables)
        {
            if (type == OP)
            {
                const int NUM_NODES = schem->nodes.size() - 1;
                Vector<double> voltage(NUM_NODES, 0.0);
                Vector<double> current(NUM_NODES, 0.0);
                Matrix<double> conductance(NUM_NODES, NUM_NODES, 0.0);
                Circuit::Math::getConductance(schem, conductance, param, 0.0, 0.0);
                Circuit::Math::getCurrent(schem, current, conductance, param, 0.0, 0.0);
                // std::cout << conductance << std::endl;
                // std::cout << current << std::endl;
                voltage = conductance.inverse() * current;
                // std::cout << voltage << std::endl;

                std::cout << "\t-----Operating Point-----\t\n\n";

                for_each(schem->nodes.begin(), schem->nodes.end(), [&](const auto node_pair) {
                    if (node_pair.second->getId() != -1)
                    {
                        node_pair.second->voltage = voltage[node_pair.second->getId()];
                        printf("V(%s)\t\t%f\tnode_voltage\n", node_pair.first.c_str(), node_pair.second->voltage);
                    }
                });

                for_each(schem->comps.begin(), schem->comps.end(), [&](const auto comp_pair) {
                    printf("I(%s)\t\t%f\tdevice_current\n", comp_pair.first.c_str(), comp_pair.second->current(param));
                });
            }
            else if (type == TRAN)
            {
                const int NUM_NODES = schem->nodes.size() - 1;
                Vector<double> voltage(NUM_NODES, 0.0);

                spicePrintTitle();
                csvPrintTitle();
                for (double t = 0; t <= tranStopTime; t += tranStepTime)
                {
                    Vector<double> current(NUM_NODES, 0.0);
                    Matrix<double> conductance(NUM_NODES, NUM_NODES, 0.0);

                    Math::getConductance(schem, conductance, param, t, tranStepTime);
                    Math::getCurrent(schem, current, conductance, param, t, tranStepTime);

                    // std::cout << conductance << std::endl;
                    // std::cout << current << std::endl;

                    voltage = conductance.inverse() * current;

                    // std::cout << voltage << std::endl;
                    
                    for_each(schem->nodes.begin(), schem->nodes.end(), [&](const auto node_pair) {
                        if (node_pair.second->getId() != -1)
                        {
                            node_pair.second->voltage = voltage[node_pair.second->getId()];
                        }
                    });

                    spicePrint(param, t, tranStepTime);
                    csvPrint(param, t, tranStepTime);
                }
                // std::cout << spiceStream.str();
                std::cout << csvStream.str();
            }
        }
    }
};
#endif
