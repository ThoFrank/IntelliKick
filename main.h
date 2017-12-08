//
// Created by thomas on 24.10.17.
//

#ifndef NEURALKICKER_MAIN_H
#define NEURALKICKER_MAIN_H


const int MAX_HIDDEN_NEURONS = 30;
const int INPUT_NEURONS = 5;
const int OUTPUT_NEURONS = 10;
const int OUTPUT_MEMORY_SIZE = 5;
const int MAX_AXON_LENGTH = 5;

/*
 * Neuron info
 * information about a neuron
 */
struct neuron_info{
    bool exist;
    double max_activation;
    double output_memory[OUTPUT_MEMORY_SIZE];
    unsigned queue_pointer;
    double bias; //used for shifting the activation function
    neuron_info();
    void enqueue(double val);
    double last_output();
    void printInfo();
};

//initialize neuron info;
void init_neuron_info();
//deallocate memory of neuron info
void del_neuron_info();


struct axon_info{
    axon_info();
    bool exist;
    double weight;
    int axon_length;
    double *axon_throughput_queue;
    unsigned queue_pointer;
    void enqueue(double val);
    void printInfo();
};

void init_Network();
void del_Network();
void init_Axon(int x, int y);



/*
 *  network matrix
 *      In1     In2     ...     Ne1     Ne2     ...
 *  Ou1
 *
 *  Ou2
 *
 *  ...
 *
 *  Ne1
 *
 *  Ne2
 *
 *  ...
 *
 *  weights[0][0] ist Gewicht der Kante In1 -> Ou1
 */

/*
 * feed forward
 */
void tick();
void learn();
//void helpLearn(int, double);


double relu_tanh(double, double, double);


neuron_info* get_input_neuron_info();
neuron_info* get_output_neuron_info();
neuron_info* get_hidden_neuron_info();
axon_info** get_network_connection_info();
//double (**get_activate())(double,double,double);

#endif //NEURALKICKER_MAIN_H
