#include <iostream>
#include <limits>
#include <cmath>
//#include <string>
#include "main.h"
#include <chrono>
#include "grapher.h"
#include <thread>

using std::cout;
using std::endl;

//neuron info for each neuron
neuron_info *input_neuron_info;
neuron_info *output_neuron_info;
neuron_info *hidden_neuron_info;

//network graph as adjacency matrix
axon_info **network_connection_info;

//activation function
double (*activate) (double, double, double);

neuron_info* get_input_neuron_info(){
    return input_neuron_info;
}

neuron_info* get_output_neuron_info(){
    return output_neuron_info;
}

neuron_info* get_hidden_neuron_info(){
    return hidden_neuron_info;
}

axon_info** get_network_connection_info(){
    return network_connection_info;
}


/*
 * Neuron info
 */
neuron_info::neuron_info() {
    queue_pointer = 0;
    exist = false;
    //queue_start = 0;
    //queue_end = OUTPUT_MEMORY_SIZE-2;
    bias=0;
}


//pushes the next output into the output_memory
void neuron_info::enqueue(double val) {
    queue_pointer = (queue_pointer+1)%OUTPUT_MEMORY_SIZE;
    output_memory[queue_pointer] = val;
}

//returns the last output inserted
double neuron_info::last_output() {
    return output_memory[queue_pointer];
}

void neuron_info::printInfo() {
    cout << "Exists: " << exist << endl
         << "Max Activation: " << max_activation << endl
         << "bias: " << bias << endl
         << "Queue: " << endl;
    //cout << output_memory[queue_start] << endl;
    for (int i = 0; i < OUTPUT_MEMORY_SIZE; i++ ) {
        cout << output_memory[(i + queue_pointer)%OUTPUT_MEMORY_SIZE] << endl;
    }
}

axon_info::axon_info(){
    exist = false;
    weight = 0;
    axon_length = 0;
    queue_pointer = 0;
}

//pushes the next output into the axon queue
void axon_info::enqueue(double val) {
    queue_pointer = (queue_pointer + 1)%axon_length;
    axon_throughput_queue[queue_pointer] = val * weight;
}

void axon_info::printInfo() {
    cout << "Exists: " << exist << endl
         << "Weight: " << weight << endl
         << "Axon Length: " << axon_length << endl
         << "Axon Queue: " << endl;
    for (int i = 0; i < axon_length; i++) {
        cout << "\t" << axon_throughput_queue[(i + queue_pointer)%axon_length] << endl;
    }
}

//initializes the connections between existing neurones
void init_Network() {
    //TODO : make generation random
    network_connection_info = new axon_info *[INPUT_NEURONS + MAX_HIDDEN_NEURONS];
    for (int x = 0; x < INPUT_NEURONS + MAX_HIDDEN_NEURONS; x++) {
        network_connection_info[x] = new axon_info[OUTPUT_NEURONS + MAX_HIDDEN_NEURONS];
        for (int y = 0; y < OUTPUT_NEURONS + MAX_HIDDEN_NEURONS; y++) {
            if (y < OUTPUT_NEURONS) {
                if (x >= INPUT_NEURONS) {
                    init_Axon(x, y);
                }
            } else {
                init_Axon(x, y);
            }
        }
    }
}

int axons = 0;

//help method
void init_Axon(int x, int y) {
    srand((unsigned) std::chrono::high_resolution_clock::now().time_since_epoch().count());
    //manipulating the exist will alter the network structure
    //-> should be manipulated by a fitting algorithm
    network_connection_info[x][y].exist =
            ((((double) random()) / RAND_MAX) * MAX_HIDDEN_NEURONS * (1 / CONNECTIVITY)) < 1;
    if (network_connection_info[x][y].exist) {
        //following should only be set if exist = true
        //initializes network with random weights (between -1 and 1)
        network_connection_info[x][y].weight = (((double) random()) / RAND_MAX) * 2 - 1;
        //initializes axon with random length (= queue length)
        network_connection_info[x][y].axon_length = (int) (1 + random() % MAX_AXON_LENGTH);
        //initializes queue; axon_length is the initial looked at destination synapsis
        network_connection_info[x][y].axon_throughput_queue =
                new double[network_connection_info[x][y].axon_length];
        network_connection_info[x][y].axon_throughput_queue[0] = 0;
        axons++;
    }
}

//deletes the connection between neurones
void del_Network(){
    for (int i = 0; i < INPUT_NEURONS + MAX_HIDDEN_NEURONS; ++i) {
        delete[] network_connection_info[i];
    }
    delete[] network_connection_info;
}

//initialize neuron info
void init_neuron_info() {
    //TODO : make generation random
    input_neuron_info = new neuron_info[INPUT_NEURONS];
    for (int i = 0; i < INPUT_NEURONS; i++) {
        input_neuron_info[i].exist = true;
        input_neuron_info[i].max_activation = std::numeric_limits<double>::max();
        for(int j = 0; j < OUTPUT_MEMORY_SIZE; j++){
            input_neuron_info[i].output_memory[j] = 0;
        }
    }

    output_neuron_info = new neuron_info[OUTPUT_NEURONS];
    for (int i = 0; i < OUTPUT_NEURONS; i++) {
        output_neuron_info[i].exist = true;
        output_neuron_info[i].max_activation = std::numeric_limits<double>::max();
        for(int j = 0; j < OUTPUT_MEMORY_SIZE; j++){
            output_neuron_info[i].output_memory[j] = 0;
        }
    }

    hidden_neuron_info = new neuron_info[MAX_HIDDEN_NEURONS];
    for (int i = 0; i < MAX_HIDDEN_NEURONS; i++) {
        hidden_neuron_info[i].exist = true;
        hidden_neuron_info[i].max_activation = std::numeric_limits<double>::max();
        for(int j = 0; j < OUTPUT_MEMORY_SIZE; j++){
            hidden_neuron_info[i].output_memory[j] = 0;
        }
    }
}

//deletes all neurones
void del_neuron_info() {
    delete[] input_neuron_info;
    delete[] output_neuron_info;
    delete[] hidden_neuron_info;
}

/*
 * feed forward
 */

//simulates the next iteration of the network
void tick() {
    //step 1: learn if there is something to learn (update all departing axons weights
    //        dependent on output_memory)
    learn();
    //step 2: collect new input -> push result in output_memory
    double buffer = 0;
    for (int y = 0; y < OUTPUT_NEURONS; y++) {
        buffer = 0;
        if (output_neuron_info[y].exist) {
            for (int x = 0; x < INPUT_NEURONS + MAX_HIDDEN_NEURONS; x++) {
                if (network_connection_info[x][y].exist) {
                    buffer += network_connection_info[x][y].axon_throughput_queue[
                            (network_connection_info[x][y].queue_pointer + 1) %
                            network_connection_info[x][y].axon_length];
                }
            }
            output_neuron_info[y].enqueue(
                    activate(buffer, output_neuron_info[y].max_activation, hidden_neuron_info[y].bias));
        }
    }
    for (int y = OUTPUT_NEURONS; y < OUTPUT_NEURONS + MAX_HIDDEN_NEURONS; y++) {
        buffer = 0;
        if (hidden_neuron_info[y - OUTPUT_NEURONS].exist) {
            for (int x = 0; x < INPUT_NEURONS + MAX_HIDDEN_NEURONS; x++) {
                if (network_connection_info[x][y].exist) {
                    buffer += network_connection_info[x][y].axon_throughput_queue[
                            (network_connection_info[x][y].queue_pointer + 1) %
                            network_connection_info[x][y].axon_length];
                }
            }
            hidden_neuron_info[y - OUTPUT_NEURONS].enqueue(
                    activate(buffer, hidden_neuron_info[y - OUTPUT_NEURONS].max_activation,
                             hidden_neuron_info[y - OUTPUT_NEURONS].bias));
        }
    }
    //step 3: push step 2s result in all departing axons
    for (int x = 0; x < INPUT_NEURONS; x++) {
        for (int y = 0; y < MAX_HIDDEN_NEURONS + OUTPUT_NEURONS; y++) {
            if (network_connection_info[x][y].exist) {
                network_connection_info[x][y].enqueue(input_neuron_info[x].last_output());
            }
        }
    }
    for (int x = INPUT_NEURONS; x < INPUT_NEURONS + MAX_HIDDEN_NEURONS; x++) {
        for (int y = 0; y < MAX_HIDDEN_NEURONS + OUTPUT_NEURONS; y++) {
            if (network_connection_info[x][y].exist) {
                network_connection_info[x][y].enqueue(hidden_neuron_info[x - INPUT_NEURONS].last_output());
            }
        }
    }
}

int feedback = 0;                       //should only be -1, 0 or 1, altering that should have an effect similiar to
                                        //slightly good or bad when below 1/-1 or very good/bad when above 1/-1

void helpLearn(int x, double total_activation) {
    double percentage_change = total_activation / (OUTPUT_MEMORY_SIZE);
    for (int y = 0; y < OUTPUT_NEURONS + MAX_HIDDEN_NEURONS; y++) {
        if (feedback < 0) {
            network_connection_info[x][y].weight -= network_connection_info[x][y].weight * percentage_change;
        } else {
            network_connection_info[x][y].weight += (1 - network_connection_info[x][y].weight) * percentage_change;
        }
    }
}

double expected_average_degree = CONNECTIVITY;     //how many incoming axons a neuron has on average
void learn() {
    if (feedback != 0) {
        for (int x = 0; x < INPUT_NEURONS; x++) {
            double total_activation = 0;
            for (int i = 0; i < OUTPUT_MEMORY_SIZE; i++) {
                total_activation = total_activation + input_neuron_info[x].output_memory[i];
            }
            helpLearn(x, total_activation);
        }
        for (int x = 0; x < MAX_HIDDEN_NEURONS; x++) {
            double total_activation = 0;
            for (int i = 0; i < OUTPUT_MEMORY_SIZE; i++) {
                total_activation = total_activation + hidden_neuron_info[x].output_memory[i];
            }
            helpLearn(x + INPUT_NEURONS, total_activation);
        }
    }
}

double relu_tanh(double in, double maxActivation, double bias) {
    if (in > 0 && in < maxActivation) {
        //return 1;
        return std::min(in + tanh(in + bias), maxActivation) == maxActivation ? maxActivation : tanh(
                in / expected_average_degree);
    } else {
        //std::cout << "ree? " << in << std::endl;
        return 0;
    }
}

void print_adjacency(){
    for(int i = 0; i < INPUT_NEURONS + MAX_HIDDEN_NEURONS; i++){
        for(int j = 0; j < OUTPUT_NEURONS + MAX_HIDDEN_NEURONS; j++){
            cout << network_connection_info[i][j].exist << " ";
        }
        cout << endl;
    }
}

/*
 * MAIN
 */
int main(int argc, char *argv[]) {
    //set activation function

    activate = relu_tanh;

    cout << "initializing neurons\n";
    init_neuron_info();
    cout << "done\n";
    cout << "initializing network\n";
    init_Network();
    cout << "done\n";
    cout << "total connections: " << axons << endl;

    print_adjacency();

    std::thread window (createWindow);

    input_neuron_info[0].output_memory[input_neuron_info[0].queue_pointer] = 1;

    std::chrono::high_resolution_clock::time_point ts1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 20; i++) {
        for (int a = 0; a < OUTPUT_NEURONS; a++) {
            cout << i << " Output: " << output_neuron_info[a].last_output() << endl;
        }
        //std::cout << "LUL\n";
        std::this_thread::sleep_for(std::chrono::duration<int>(1));
        tick();
        input_neuron_info[0].output_memory[input_neuron_info[0].queue_pointer] = 0;
    }
    std::chrono::high_resolution_clock::time_point ts2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(ts2 - ts1);
    cout << "tick time: " << time_span.count()/20 << endl;

    window.join();
    cout << "window closed" << endl;
    del_neuron_info();
    del_Network();
    return 0;
}