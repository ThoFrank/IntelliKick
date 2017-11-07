#include <iostream>
#include <limits>
#include <math.h>
#include "main.h"



/*
 *  weight matrix
 */

void init_weights(){
    weights=new double*[INPUT_NEURONS+MAX_HIDDEN_NEURONS];
    for (int x = 0; x < INPUT_NEURONS + MAX_HIDDEN_NEURONS; ++x) {
        weights[x]=new double[OUTPUT_NEURONS + MAX_HIDDEN_NEURONS];
        for (int y = 0; y < OUTPUT_NEURONS + MAX_HIDDEN_NEURONS; ++y) {
            weights[x][y]=0;
        }
    }
}

void del_weights(){
    for (int i = 0; i < INPUT_NEURONS + MAX_HIDDEN_NEURONS; ++i) {
        delete[] weights[i];
    }
    delete[] weights;
}

/*
 * Neuron info
 */
neuron_info::neuron_info() {
    queue_start=0;
    queue_end=OUTPUT_MEMORY_SIZE-2;
}
void neuron_info::queue(double val) {
    queue_end=(queue_end+1)%OUTPUT_MEMORY_SIZE;
    output_memory[queue_end];
}
double neuron_info::dequeue() {
    double ret=output_memory[queue_start];
    queue_start=(queue_start+1)%OUTPUT_MEMORY_SIZE;
    return ret;
}
double neuron_info::last_output() {
    return output_memory[queue_end];
}

//initialize neuron info
void init_neuron_info(){
    input_neuron_info=new neuron_info[INPUT_NEURONS];
    std::cout << sizeof(neuron_info) << std::endl;
    for (int i = 0; i < INPUT_NEURONS; ++i) {
        input_neuron_info[i].exist=true;
        input_neuron_info[i].max_activation=std::numeric_limits<double>::max();
    }

    output_neuron_info=new neuron_info[OUTPUT_NEURONS];
    for (int i = 0; i < OUTPUT_NEURONS; ++i) {
        output_neuron_info[i].exist=true;
        output_neuron_info[i].max_activation=std::numeric_limits<double>::max();
    }

    hidden_neuron_info=new neuron_info[MAX_HIDDEN_NEURONS];
    for (int i = 0; i < MAX_HIDDEN_NEURONS; ++i) {
        output_neuron_info[i].exist=false;
        output_neuron_info[i].max_activation=0;
    }
}

void del_neuron_info(){
    delete[] input_neuron_info;
    delete[] output_neuron_info;
    delete[] hidden_neuron_info;
}

void update_connection(int actor_neuron, int reciever_neuron, double weight){
    weights[actor_neuron][reciever_neuron]=weight;
}

/*
 * feed forward
 */
void tick(){
    double buffer[OUTPUT_NEURONS+MAX_HIDDEN_NEURONS];

    for (int y = 0; y < OUTPUT_NEURONS; ++y) {
        buffer[y]=0;
        for (int x = 0; x < INPUT_NEURONS; ++x) {
            if(input_neuron_info[x].exist){
                buffer[y] += weights[x][y] * input_neuron_info[x].last_output();
            }
        }
        for (int x = INPUT_NEURONS; x < MAX_HIDDEN_NEURONS+INPUT_NEURONS; ++x) {
            if(hidden_neuron_info[x].exist){
                buffer[y] += weights[x][y] * hidden_neuron_info[x].last_output();
            }
        }
    }
    for (int y = OUTPUT_NEURONS; y < OUTPUT_NEURONS+MAX_HIDDEN_NEURONS; ++y) {
        buffer[y]=0;
        for (int x = 0; x < INPUT_NEURONS; ++x) {
            if(input_neuron_info[x].exist){
                buffer[y]+=weights[x][y] * input_neuron_info[x].last_output();
            }
        }
        for (int x = INPUT_NEURONS; x < MAX_HIDDEN_NEURONS+INPUT_NEURONS; ++x) {
            if(hidden_neuron_info[x].exist){
                buffer[y]+=weights[x][y] * hidden_neuron_info[x].last_output();
            }
        }
    }

    //add bias; activate and enqueue buffer
    for (int y = 0; y < OUTPUT_NEURONS; ++y) {
        buffer[y]+=output_neuron_info[y].bias;
        output_neuron_info[y].queue( activate(buffer[y]) );
    }
    for (int y = OUTPUT_NEURONS; y < OUTPUT_NEURONS + MAX_HIDDEN_NEURONS; ++y) {
        buffer[y]+=output_neuron_info[y].bias;
        hidden_neuron_info[y].queue( activate(buffer[y]) );
    }
}

double relu_tanh(double in){
    if(in>0){
        return tanh(in);
    }else{
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
    init_weights();

    for (int i = 0; i < OUTPUT_NEURONS; ++i) {
        update_connection(0,i,1);
    }

    tick();

    del_weights();
    del_neuron_info();
    return 0;
}