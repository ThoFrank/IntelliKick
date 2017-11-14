#include <iostream>
#include <limits>
#include <math.h>
#include <string>
#include "main.h"

using std::cout;
using std::endl;

/*
 *  weight matrix
 */
/*void init_weights(){
    weights = new double*[INPUT_NEURONS+MAX_HIDDEN_NEURONS];
    for (int x = 0; x < INPUT_NEURONS + MAX_HIDDEN_NEURONS; ++x) {
        weights[x] = new double[OUTPUT_NEURONS + MAX_HIDDEN_NEURONS];
        for (int y = 0; y < OUTPUT_NEURONS + MAX_HIDDEN_NEURONS; ++y) {
            weights[x][y] = (((double) rand())/RAND_MAX)*2-1;
        }
    }
}

void del_weights(){
    for (int i = 0; i < INPUT_NEURONS + MAX_HIDDEN_NEURONS; ++i) {
        delete[] weights[i];
    }
    delete[] weights;
}*/

/*
 * Neuron info
 */
neuron_info::neuron_info() {
    queue_pointer = 0;
    //queue_start = 0;
    //queue_end = OUTPUT_MEMORY_SIZE-2;
    bias=0;
}


//pushes the next output into the output_memory
void neuron_info::enqueue(double val) {
    queue_pointer = (queue_pointer+1)%OUTPUT_MEMORY_SIZE;
    output_memory[queue_pointer] = val;
}

/*void neuron_info::queue(double val) {
    queue_end=(queue_end+1)%OUTPUT_MEMORY_SIZE;
    output_memory[queue_end]=val;
}
//zusammenfassbar mit queue
double neuron_info::dequeue() {
    double ret=output_memory[queue_start];
    queue_start=(queue_start+1)%OUTPUT_MEMORY_SIZE;
    return ret;
}*/

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

//pushes the next output into the axon queue
void axon_info::enqueue(double val) {
    queue_pointer = (queue_pointer + 1)%axon_length;
    axon_throughtput_queue[queue_pointer] = val;
}

void axon_info::printInfo() {
    cout << "Exists: " << exist << endl
         << "Weight: " << weight << endl
         << "Axon Length: " << axon_length << endl
         << "Axon Queue: " << endl;
    for (int i = 0; i < axon_length; i++) {
        cout << "\t" << axon_throughtput_queue[(i + queue_pointer)%axon_length] << endl;
    }
}

//initializes the connections between existing neurones
void initNetwork(){
    network_connection_info = new axon_info *[INPUT_NEURONS+MAX_HIDDEN_NEURONS];
    for (int x = 0; x < INPUT_NEURONS + MAX_HIDDEN_NEURONS; x++) {
        network_connection_info[x] = new axon_info[OUTPUT_NEURONS + MAX_HIDDEN_NEURONS];
        cout << "test4 " << &network_connection_info[x][11] << endl;
        for (int y = 0; y < OUTPUT_NEURONS + MAX_HIDDEN_NEURONS; y++) {
            if(y < OUTPUT_NEURONS){
                if(output_neuron_info[y].exist){
                    initializeAxon(x, y);
                }
            }else {
                if(hidden_neuron_info[y].exist){
                    initializeAxon(x, y);
                }
            }
        }
    }
    cout << "test3 " << &network_connection_info[2][11] << endl;
}

//help method
void initializeAxon(int x, int y){
    //manipulating the exist will alter the network structure
    //-> should be manipulated by a fitting algorithm
    network_connection_info[x][y].exist = true;
    //following should only be set if exist = true
    //initializes network with random weights (between -1 and 1)
    network_connection_info[x][y].weight = (((double) rand())/RAND_MAX)*2-1;
    //initializes axon with random length (= queue length)
    network_connection_info[x][y].axon_length = 1 + rand()%(MAX_AXION_LENGTH-1);
    //initializes queue; axon_length is the initial looked at destination synapsis
    network_connection_info[x][y].axon_throughtput_queue =
            new double[network_connection_info[x][y].axon_length];
    network_connection_info[x][y].queue_pointer = 0;
}

//deletes the connection between neurones
void del_Network(){
    for (int i = 0; i < INPUT_NEURONS + MAX_HIDDEN_NEURONS; ++i) {
        delete[] network_connection_info[i];
    }
    delete[] network_connection_info;
}

//initialize neuron info
void init_neuron_info(){
    //TODO : make generation random
    input_neuron_info=new neuron_info[INPUT_NEURONS];
    for (int i = 0; i < INPUT_NEURONS; ++i) {
        input_neuron_info[i].exist = true;
        input_neuron_info[i].max_activation = std::numeric_limits<double>::max();
    }

    output_neuron_info=new neuron_info[OUTPUT_NEURONS];
    for (int i = 0; i < OUTPUT_NEURONS; ++i) {
        output_neuron_info[i].exist = true;
        output_neuron_info[i].max_activation = std::numeric_limits<double>::max();
    }

    hidden_neuron_info=new neuron_info[MAX_HIDDEN_NEURONS];
    for (int i = 0; i < MAX_HIDDEN_NEURONS; ++i) {
        hidden_neuron_info[i].exist = true;
        hidden_neuron_info[i].max_activation = std::numeric_limits<double>::max();
    }
}

//deletes all neurones
void del_neuron_info(){
    delete[] input_neuron_info;
    delete[] output_neuron_info;
    delete[] hidden_neuron_info;
}

//unnötig, sollte effizienter in der ausführenden funktion gelöst werden können
/*void update_connection(int actor_neuron, int reciever_neuron, double weight){
    weights[actor_neuron][reciever_neuron] = weight;
}*/

/*
 * feed forward
 */

//simulates the next iteration of the network
void tick(){
    //step 1: learn if there is something to learn (update all departing axons weights
    //        dependent on output_memory)
    learn();
    //step 2: collect new input -> push result in output_memory
    cout << "test3 " << &network_connection_info[0][11] << endl;
    double buffer = 0;
    for (int y = 0; y < OUTPUT_NEURONS; y++) {
        buffer = 0;
        for (int x = 0; x < INPUT_NEURONS + MAX_HIDDEN_NEURONS; x++) {
            if(network_connection_info[x][y].exist){
                buffer += network_connection_info[x][y].axon_throughtput_queue[
                        (network_connection_info[x][y].queue_pointer-1)%network_connection_info[x][y].axon_length];
            }
        }
        output_neuron_info[y].enqueue(activate(buffer, hidden_neuron_info[y].max_activation, hidden_neuron_info[y].bias));
    }
    for (int y = OUTPUT_NEURONS; y < OUTPUT_NEURONS+MAX_HIDDEN_NEURONS; ++y) {
        buffer = 0;
        for (int x = 0; x < INPUT_NEURONS + MAX_HIDDEN_NEURONS; x++) {
            cout << "test2 " << &network_connection_info[x][y] << " " << x << " " << y << endl;
            if(network_connection_info[x][y].exist){
                buffer += network_connection_info[x][y].axon_throughtput_queue[
                        (network_connection_info[x][y].queue_pointer-1)%network_connection_info[x][y].axon_length];
            }
        }
        cout << "test " << buffer << endl;
        hidden_neuron_info[y].enqueue(activate(buffer, hidden_neuron_info[y].max_activation, hidden_neuron_info[y].bias));
    }
    //step 3: push step 2s result in all departing axons
    for (int x = 0; x < INPUT_NEURONS; x++) {
        for (int y = 0; y < MAX_HIDDEN_NEURONS + OUTPUT_NEURONS; y++) {
            if (network_connection_info[x][y].exist) {
                network_connection_info[x][y].enqueue(input_neuron_info[y].last_output());
            }
        }
    }
    for (int x = INPUT_NEURONS; x < INPUT_NEURONS + MAX_HIDDEN_NEURONS; x++) {
        for (int y = 0; y < MAX_HIDDEN_NEURONS + OUTPUT_NEURONS; y++) {
            if (network_connection_info[x][y].exist) {
                network_connection_info[x][y].enqueue(input_neuron_info[y].last_output());
            }
        }
    }
}

void learn() {
    //TODO
}

/*void tick(){
    double buffer[OUTPUT_NEURONS+MAX_HIDDEN_NEURONS];
    for (int y = 0; y < OUTPUT_NEURONS; ++y) {
        buffer[y] = 0;
        for (int x = 0; x < INPUT_NEURONS; ++x) {
            if(input_neuron_info[x].exist){
                buffer[y] += weights[x][y] * input_neuron_info[x].last_output();
            }
        }
        for (int x = INPUT_NEURONS; x < MAX_HIDDEN_NEURONS+INPUT_NEURONS; ++x) {
            if(hidden_neuron_info[x].exist){
                buffer[y] += weights[x][y] * hidden_neuron_info[x-INPUT_NEURONS].last_output();
            }
        }
    }
    for (int y = OUTPUT_NEURONS; y < OUTPUT_NEURONS+MAX_HIDDEN_NEURONS; ++y) {
        buffer[y]=0;
        for (int x = 0; x < INPUT_NEURONS; ++x) {
            if(input_neuron_info[x].exist){
                buffer[y] += weights[x][y] * input_neuron_info[x].last_output();
            }
        }
        for (int x = INPUT_NEURONS; x < MAX_HIDDEN_NEURONS+INPUT_NEURONS; ++x) {
            if(hidden_neuron_info[x].exist){
                buffer[y] += weights[x][y] * hidden_neuron_info[x-INPUT_NEURONS].last_output();
            }
        }
    }

    //add bias; activate and enqueue buffer
    //TODO use MaxActivation
    //Variante 1: Maxactivation überschritten -> keine Aktivierung / Aktivierung wird ausgesetzt wenn max_activation überschritten werden würde
    //Variante 2: Maxactivation überschritten -> gedeckelte Aktivierung / Aktivierung bis max_activation
    for (int y = 0; y < OUTPUT_NEURONS; ++y) {
        buffer[y] += output_neuron_info[y].bias;
        output_neuron_info[y].enqueue( activate(buffer[y], output_neuron_info[y].max_activation) );
    }
    for (int y = 0; y < MAX_HIDDEN_NEURONS; ++y) {
        buffer[y+OUTPUT_NEURONS] += hidden_neuron_info[y].bias;
        hidden_neuron_info[y].enqueue( activate(buffer[y+OUTPUT_NEURONS], hidden_neuron_info[y].max_activation) );
    }
}*/

double relu_tanh(double in, double maxActivation, double bias){
    if(in > 0 && in < maxActivation) {
        return std::max(in + tanh(in + bias), maxActivation) == maxActivation ? maxActivation : tanh(in);
    } else {
        return 0;
    }
}


/*
 * MAIN
 */
int main(int argc, char *argv[]) {
    //set activation function
    activate=relu_tanh;

    init_neuron_info();
    //init_weights();
    initNetwork();

    /*or (int i = 0; i < OUTPUT_NEURONS; ++i) {
        update_connection(0,i,1);
    }*/

    /*for (int i = 0; i < 5; ++i) {
        input_neuron_info[0].output_memory[input_neuron_info[0].queue_pointer] = i / 5;
        cout << "Tick " << i << endl;
        cout << "input neuron 0:" << endl;
        input_neuron_info[0].printInfo();
        cout << endl;
        cout << "output neuron 0:" << endl;
        output_neuron_info[0].printInfo();
        cout << endl;
        tick();
    }*/

    for (int i = 0; i < 9; i++) {
        input_neuron_info[0].output_memory[input_neuron_info[0].queue_pointer] = i / 5;
        for (int a = 0; a < OUTPUT_NEURONS; a++) {
            cout << i << " Output: " << output_neuron_info[a].last_output() << endl;
        }
        tick();
    }

    //del_weights();
    del_neuron_info();
    del_Network();
    return 0;
}