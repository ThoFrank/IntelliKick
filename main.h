//
// Created by thomas on 24.10.17.
//

#ifndef NEURALKICKER_MAIN_H
#define NEURALKICKER_MAIN_H


const unsigned MAX_HIDDEN_NEURONS = 10;
const unsigned INPUT_NEURONS = 5;
const unsigned OUTPUT_NEURONS = 8;
const unsigned OUTPUT_MEMORY_SIZE = 5;
const unsigned MAX_AXION_LENGTH = 10;

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
//neuron info for each neuron
neuron_info *input_neuron_info;
neuron_info *output_neuron_info;
neuron_info *hidden_neuron_info;
//initialize neuron info;
void init_neuron_info();
//deallocate memory of neuron info
void del_neuron_info();


struct axon_info{
    bool exist = false;
    double weight;
    int axon_length;
    double *axon_throughtput_queue;
    unsigned queue_pointer;
    void enqueue(double val);
    void printInfo();
};
//network graph as adjacency matrix
axon_info **network_connection_info;
void initNetwork();
void del_Network();
void initializeAxon(int x, int y);



/*
 *  weight matrix
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
/*double **weights;
//initialize empty weight matix (allocate storage)
void init_weights();
//deallocate memory of weight matrix
void del_weights();
//add connection between neurons; returns 0 if successfull
void update_connection(int actor_neuron, int reciever_neuron, double weight);*/


/*
 * feed forward
 */
void tick();
void learn();
//activation function
double (*activate) (double, double, double);

//double activate();

#endif //NEURALKICKER_MAIN_H
