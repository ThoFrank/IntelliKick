//
// Created by sebastian on 21.11.17.
//

#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include "main.h"
#include "grapher.h"
#include <cmath>

sf::RenderWindow window;
sf::Font font;

std::chrono::duration<double> frame;

struct node{
    //TODO: graphical layout and details
};

struct graph {
    node *nodes;
    int **structure;
    double **adjacency;
    int *size;
    int total_connections;
    int test;
};

//tests wether a given node(id) is part of the given structure and returns true if it exists with the position where it
//exists or false if it doesn't exist
int* exist(int** structure, int id) {
    int *results = new int[3];
    for (int x = 0; x < INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS; x++) {
        for (int y = 0; y < INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS; y++) {
            if (structure[x][y] == id) {
                results[0] = 1;
                results[1] = x;
                results[2] = y;
                return results;
            }
        }
    }
    return results;
}


//waits for inputs and returns catched input
double* getInput(double *paras) {
    while (1) {
        std::chrono::high_resolution_clock::time_point ts1 = std::chrono::high_resolution_clock::now();
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    return paras;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::Down:
                            paras[1] = paras[1] - 5;
                            return paras;
                        case sf::Keyboard::Up:
                            paras[1] = paras[1] + 5;
                            return paras;
                        case sf::Keyboard::Left:
                            paras[0] = paras[0] + 5;
                            return paras;
                        case sf::Keyboard::Right:
                            paras[0] = paras[0] - 5;
                            return paras;
                        case sf::Keyboard::Add:
                            paras[2] *= 1.01;
                            return paras;
                        case sf::Keyboard::Subtract:
                            paras[2] /= 1.01;
                            return paras;
                        default:
                            break;
                    }
                default:
                    break;
            }
        }
        std::chrono::high_resolution_clock::time_point ts2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(ts2 - ts1);
        if (time_span.count() < 1.f / 30.f) {
            std::this_thread::sleep_for(frame - time_span);
        }
    }
}

//transforms the given network into a flow-tree structure
void helpCreateGraph(graph* network, int start, bool complete) {
    int *from = new int[INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS];
    int *to = new int[INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS];
    from[0] = start;
    network->structure[0][start] = start + 1;
    int starting_points_old = 1;
    int starting_points_new = 0;
    int pos_x = 1;
    while (1) {
        for (int i = 0; i < starting_points_old; i++) {
            if (from[i] != -1) {
                for (int j = 0; j < MAX_HIDDEN_NEURONS + OUTPUT_NEURONS; j++) {
                    if (get_network_connection_info()[from[i]][j].exist) {
                        network->adjacency[network->total_connections] = new double[5];
                        network->adjacency[network->total_connections][0] = get_network_connection_info()[from[i]][j].weight;
                        network->adjacency[network->total_connections][1] = pos_x - 1;
                        network->adjacency[network->total_connections][2] = pos_x == 1 ? start : i;
                        int *exists = exist(network->structure, INPUT_NEURONS + j + 1);
                        if (exists[0] != 1) {
                            int test = 0;
                            while(network->structure[pos_x][starting_points_new+test] != 0){
                                test++;
                            }
                            if(network->structure[pos_x][starting_points_new+test] != 0) std::cout << INPUT_NEURONS + j + 1 << " " << network->structure[pos_x][starting_points_new] << std::endl;
                            network->adjacency[network->total_connections][3] = pos_x;
                            network->adjacency[network->total_connections][4] = starting_points_new+test;
                            network->structure[pos_x][starting_points_new+test] = INPUT_NEURONS + j + 1;
                            if (j >= OUTPUT_NEURONS) {
                                to[starting_points_new] = INPUT_NEURONS + (j - OUTPUT_NEURONS);
                            } else {
                                to[starting_points_new] = -1;
                            }
                            starting_points_new++;
                        } else {
                            network->adjacency[network->total_connections][3] = complete ? exists[1] : pos_x - 1;
                            network->adjacency[network->total_connections][4] = complete ? exists[2] : pos_x == 1
                                                                                                       ? start : i;
                        }
                        network->total_connections++;
                    }
                }
            }
        }
        if (starting_points_new == 0) return;
        for (int i = 0; i < starting_points_new; i++) {
            from[i] = to[i];
        }
        starting_points_old = starting_points_new;
        starting_points_new = 0;
        pos_x++;
    }
}

//calculates the y coordinates of each node to get a visually more appealing flow-tree
//TODO: adjacency could already be finished in helpCreateTree()
int helpCreateTree(graph* network, int** newStructure, double** newAdjacency, int pos_x, int pos_y, int newPos, int* connections) {
    int factor = 0;
    for (int i = 0; i < network->total_connections; i++) {
        if (network->adjacency[i][1] == pos_x && network->adjacency[i][2] == pos_y) {
            connections[0]++;
            newAdjacency[connections[0]] = new double[5];
            newAdjacency[connections[0]][0] = network->adjacency[i][0];
            newAdjacency[connections[0]][1] = network->structure[pos_x][pos_y];
            int *exists = exist(network->structure,
                                network->structure[(int) network->adjacency[i][3]][(int) network->adjacency[i][4]]);
            newAdjacency[connections[0]][3] = network->structure[exists[1]][exists[2]];
            exists = exist(newStructure,
                           network->structure[(int) network->adjacency[i][3]][(int) network->adjacency[i][4]]);
            if (exists[0] != 1 && network->adjacency[i][3] == pos_x + 1) {
                factor += helpCreateTree(network, newStructure, newAdjacency, network->adjacency[i][3],
                                         network->adjacency[i][4], newPos + factor, connections);
            }
        }
    }
    newStructure[pos_x][newPos + factor / 2] = network->structure[pos_x][pos_y];
    if (pos_x == 0) network->test += factor;
    return factor == 0 ? 1 : factor;
}

//tells the connections the structure of the flow tree
void layConnections(graph* network) {
    for (int i = 0; i < network->total_connections; i++) {
        int *exists1 = exist(network->structure, (int) network->adjacency[i][1]);
        int *exists2 = exist(network->structure, (int) network->adjacency[i][3]);
        network->adjacency[i][1] = exists1[1];
        network->adjacency[i][2] = exists1[2];
        network->adjacency[i][3] = exists2[1];
        network->adjacency[i][4] = exists2[2];
    }
}

//creates a new graph representing the neural network as flow-tree
graph* createGraph() {
    graph *network = new graph;
    network->nodes = new node[INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS];
    network->structure = new int *[INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS];
    network->adjacency = new double *[(INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS) * MAX_HIDDEN_NEURONS];
    double **newAdjacency = new double *[(INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS) * MAX_HIDDEN_NEURONS];
    int **newStructure = new int *[INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS];
    for (int i = 0; i < INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS; i++) {
        network->structure[i] = new int[INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS];
        newStructure[i] = new int[INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS];
    }
    /*for (int i = 0; i < INPUT_NEURONS; i++) {
        helpCreateGraph(network, i, complete);
    }*/
    for (double i = 0; i < INPUT_NEURONS; i++) {
        //helpCreateGraph(network, i, complete);
        if ((int)i%2){
            helpCreateGraph(network, INPUT_NEURONS/2-i/2, complete);
        } else{
            helpCreateGraph(network, INPUT_NEURONS/2+i/2, complete);
        }
    }
    int *connections = new int[1];
    connections[0] = -1;
    for (int i = 0; i < INPUT_NEURONS; i++) {
        helpCreateTree(network, newStructure, newAdjacency, 0, i, i + network->test, connections);
    }
    network->total_connections = connections[0] + 1;
    network->structure = newStructure;
    network->adjacency = newAdjacency;
    layConnections(network);
    return network;
}

//draws the graph
//TODO: make better zoom
void draw(double* paras, graph* network) {
    //draw connections
    for (int i = 0; i < network->total_connections; i++) {
        double *from = new double[2];
        double *to = new double[2];
        from[0] = network->adjacency[i][1] * 2 * paras[2] + 10 + paras[0];
        from[1] = network->adjacency[i][2] * paras[2] + 10 + paras[1];
        to[0] = network->adjacency[i][3] * 2 * paras[2] + 10 + paras[0];
        to[1] = network->adjacency[i][4] * paras[2] + 10 + paras[1];
        double norm = std::sqrt((from[0] - to[0]) * (from[0] - to[0]) + (from[1] - to[1]) * (from[1] - to[1]));
        double *normV = new double[2];
        normV[0] = (to[0] - from[0]) / norm;
        normV[1] = (to[1] - from[1]) / norm;
        double norm2 = (std::abs(to[0] - from[0])) / norm;
        if (norm != 0) {
            sf::Color c = sf::Color(network->adjacency[i][0] > 0 ? 0 : network->adjacency[i][0] * (-150) + 105,
                                    network->adjacency[i][0] > 0 ? network->adjacency[i][0] * 150 + 105 : 0, 0);
            sf::Vertex lineM[] = {
                    sf::Vertex(sf::Vector2f(from[0], from[1]), c),
                    sf::Vertex(sf::Vector2f(to[0], to[1]), c),
            };
            sf::Vertex lineL[] = {
                    sf::Vertex(sf::Vector2f(to[0] - normV[0] * 10, to[1] - normV[1] * 10), c),
                    sf::Vertex(sf::Vector2f(to[0] - normV[0] * 20 -
                                            (normV[1] / (normV[0] == 0 ? 1 : normV[0])) * (norm2 == 0 ? 1 : norm2) * 10,
                                            to[1] - normV[1] * 20 + 1 * norm2 * 10), c),
            };
            sf::Vertex lineR[] = {
                    sf::Vertex(sf::Vector2f(to[0] - normV[0] * 10, to[1] - normV[1] * 10), c),
                    sf::Vertex(sf::Vector2f(to[0] - normV[0] * 20 +
                                            (normV[1] / (normV[0] == 0 ? 1 : normV[0])) * (norm2 == 0 ? 1 : norm2) * 10,
                                            to[1] - normV[1] * 20 - 1 * norm2 * 10), c),
            };
            window.draw(lineM, 2, sf::Lines);
            window.draw(lineL, 2, sf::Lines);
            window.draw(lineR, 2, sf::Lines);
        }
    }
    //draw nodes
    for (int x = 0; x < INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS; x++) {
        for (int y = 0; y < INPUT_NEURONS + MAX_HIDDEN_NEURONS + OUTPUT_NEURONS; y++) {
            if (network->structure[x][y] != 0) {
                sf::CircleShape c;
                sf::Text id;
                id.setFont(font);
                if (network->structure[x][y] > INPUT_NEURONS &&
                    network->structure[x][y] <= INPUT_NEURONS + OUTPUT_NEURONS) {
                    c.setFillColor(sf::Color(0, 0, 255));
                    id.setString(std::to_string((network->structure[x][y] - INPUT_NEURONS - 1)));
                } else if (x == 0) {
                    c.setFillColor(sf::Color(200, 200, 0));
                    id.setString(std::to_string((network->structure[x][y] - 1)));
                } else {
                    c.setFillColor(sf::Color(100, 100, 100));
                    id.setString(std::to_string((network->structure[x][y] - (INPUT_NEURONS + OUTPUT_NEURONS) - 1)));
                }
                c.setRadius(10);
                c.setPosition(x * 2 * paras[2] + paras[0], y * paras[2] + paras[1]);
                id.setPosition(x * 2 * paras[2] + 5 + paras[0], y * paras[2] + paras[1]);
                id.setCharacterSize(15);
                id.setFillColor(sf::Color::White);
                window.draw(c);
                window.draw(id);
            }
        }
    }
}

//initializes window
//use arrow keys to move the graph, add/substract key to zoom
int createWindow() {
    graph* network = createGraph();
    double* paras = new double[3];
    paras[2] = 50; //initial zoom factor
    window.create(sf::VideoMode(1900, 1080), "Network Graph");
    if (!font.loadFromFile("./fonts/arial.ttf"))
    {
        std::cout << "failed to load arial.ttf\n";
    }else{
        std::cout << "successfully loaded arial.ttf!\n";
    }
    std::chrono::high_resolution_clock::time_point ts1 = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point ts2 = std::chrono::high_resolution_clock::now();
    while (std::chrono::duration_cast<std::chrono::duration<double>>(ts2 - ts1).count() < 1.f / 30.f)
        ts2 = std::chrono::high_resolution_clock::now();
    frame = std::chrono::duration_cast<std::chrono::duration<double>>(ts2 - ts1);
    window.clear(sf::Color(255,255,255));
    draw(paras, network);
    window.display();
    //main loop
    while (window.isOpen()) {
        window.clear(sf::Color(255,255,255));
        draw(getInput(paras), network);
        window.display();
    }
    ts2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(ts2 - ts1);
    std::cout << "window was open for: " << time_span.count() << " seconds" << std::endl;
    window.close();

    return 0;
}