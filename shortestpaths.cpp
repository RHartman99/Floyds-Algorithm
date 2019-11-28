/*******************************************************************************
 * Name    : shortestpaths.cpp
 * Author  : Ryan J. Hartman & Daniel Pekata
 * Version : 1.0
 * Date    : December 3rd, 2019
 * Description : Uses Floyd's Algorithm to find the shortest path to each vertex in a graph.
 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <climits>

using namespace std;

int len(int val, bool isChar)
{
    if (isChar)
        return 1;

    int length = 0;

    while (val > 0)
    {
        val = val / 10;
        length++;
    }

    return length;
}

void display_table(long **const matrix, int num_vertices, const string &label,
                   const bool use_letters = false)
{
    cout << label << endl;
    long max_val = 0;
    for (int i = 0; i < num_vertices; i++)
    {
        for (int j = 0; j < num_vertices; j++)
        {
            long cell = matrix[i][j];
            if (cell <= INT_MAX && cell > max_val)
            {
                max_val = matrix[i][j];
            }
        }
    }
    int max_cell_width = use_letters ? len(max_val, use_letters) : len(max(static_cast<long>(num_vertices), max_val), use_letters);
    cout << ' ';
    for (int j = 0; j < num_vertices; j++)
    {
        cout << setw(max_cell_width + 1) << static_cast<char>(j + 'A');
    }
    cout << endl;
    for (int i = 0; i < num_vertices; i++)
    {
        cout << static_cast<char>(i + 'A');
        for (int j = 0; j < num_vertices; j++)
        {
            cout << " " << setw(max_cell_width);
            if (matrix[i][j] > INT_MAX)
            {
                cout << "-";
            }
            else if (use_letters)
            {
                cout << static_cast<char>(matrix[i][j]);
            }
            else
            {
                cout << matrix[i][j];
            }
        }
        cout << endl;
    }
    cout << endl;
}

bool validate_input(ifstream &matrix_file)
{
    string line, v_1, v_2, edge_str;
    int line_number = 0, value = 0, edge_weight = 0, max_ascii = 0;
    // long **distance_matrix;
    while (getline(matrix_file, line))
    {
        line_number++;
        if (line_number == 1)
        {
            istringstream iss(line);
            if (!(iss >> value) || value <= 0 || value > 26)
            {
                cerr << "Error: Invalid number of vertices '" << line << "' on line 1.";
                return false;
            }
            max_ascii = 'A' + value - 1;
        }
        else
        {
            stringstream sstream(line);

            if (!(sstream >> v_1 >> v_2 >> edge_str))
            {
                cerr << "Error: Invalid edge data '" << line << "' on line " << line_number << ".";
                return false;
            }

            istringstream iss(edge_str);

            if (!(iss >> edge_weight) || edge_weight <= 0)
            {
                cerr << "Error: Invalid edge weight '" << edge_str << "' on line " << line_number << ".";
                return false;
            }

            if (v_1.length() > 1 || v_1[0] < 'A' || v_1[0] > max_ascii)
            {
                cerr << "Error: Starting vertex '" << v_1 << "' on line " << line_number << " is not among valid values A-" << char(max_ascii) << ".";
                return false;
            }

            if (v_2.length() > 1 || v_2[0] < 'A' || v_2[0] > max_ascii)
            {
                cerr << "Error: Ending vertex '" << v_2 << "' on line " << line_number << " is not among valid values A-" << char(max_ascii) << ".";
                return false;
            }

            if (iss >> line)
            {
                cerr << "Error: Too many entries on line '" << line_number << "'.";
                return false;
            }
        }
    }
    return true;
}

void print_ways(int i, int j, long** paths_matrix, long** intermediate_matrix) {
    if(paths_matrix[i][j] < INT_MAX && paths_matrix[i][j] != 0) {
        cout << "1: " << intermediate_matrix[i][j] << " 2: " << intermediate_matrix[i][j]; 
        // print_ways(i, intermediate_matrix[i][j] - 'A', paths_matrix, intermediate_matrix);
        // print_ways(intermediate_matrix[i][j] - 'A', j, paths_matrix, intermediate_matrix);
    }
    cout << " " << char(intermediate_matrix[i][j]) << " ->";
}

bool findPaths(ifstream &matrix_file)
{
    if (!validate_input(matrix_file))
        return false;

    stringstream sstream;
    istringstream iss;
    string line, edge_str;
    char v_1, v_2;
    int num_vertices = 0, value;

    matrix_file.clear();
    matrix_file.seekg(0, ios::beg);
    getline(matrix_file, line);

    iss.str(line);
    iss >> num_vertices;

    long **distance_matrix = new long *[num_vertices];
    long **paths_matrix = new long *[num_vertices];
    long **intermediate_matrix = new long *[num_vertices];

    for (int i = 0; i < num_vertices; i++)
    {
        distance_matrix[i] = new long[num_vertices];
        paths_matrix[i] = new long[num_vertices];
        intermediate_matrix[i] = new long[num_vertices];
        fill_n(distance_matrix[i], num_vertices, (long)INT_MAX + 10);
        fill_n(paths_matrix[i], num_vertices, (long)INT_MAX + 10);
        fill_n(intermediate_matrix[i], num_vertices, (long)INT_MAX + 10);
        distance_matrix[i][i] = 0;
        paths_matrix[i][i] = 0;
    }

    while (getline(matrix_file, line))
    {
        sstream.str(line);
        sstream.clear();
        sstream >> v_1 >> v_2 >> edge_str;
        iss.str(edge_str);
        iss.clear();
        iss >> value;
        distance_matrix[v_1 - 'A'][v_2 - 'A'] = value;
        paths_matrix[v_1 - 'A'][v_2 - 'A'] = value;
    }

    // Apply Floyd's Algorithm:
    for (int k = 0; k < num_vertices; k++)
        for (int i = 0; i < num_vertices; i++)
            for (int j = 0; j < num_vertices; j++)
                if (paths_matrix[i][k] + paths_matrix[k][j] < paths_matrix[i][j] && i != j)
                {
                    paths_matrix[i][j] = paths_matrix[i][k] + paths_matrix[k][j];
                    intermediate_matrix[i][j] = (int)k + 'A';
                }

    display_table(distance_matrix, num_vertices, "Distance matrix:");
    display_table(paths_matrix, num_vertices, "Path lengths:");
    display_table(intermediate_matrix, num_vertices, "Intermediate vertices:", true);
    for (int i = 0; i < num_vertices; i++)
        for (int j = 0; j < num_vertices; j++) {
            if(paths_matrix[i][j] == 0) {
                cout << endl << char(i + 'A') << " -> " << char(j + 'A') << ", distance: " << paths_matrix[i][j] << ", path: " << char(i + 'A');
            }
            else if (paths_matrix[i][j] <= INT_MAX)
            {
                cout << endl << char(i + 'A') << " -> " << char(j + 'A') << ", distance: " << paths_matrix[i][j] << ", path: " << char(i + 'A') << " ->";
                print_ways(i, j, paths_matrix, intermediate_matrix);
            }
            else
                cout << endl << i + 'A' << " -> " << j + 'A' << ", distance: infinity, path: none";
        }

    // Cleanup
    for (int i = 0; i < num_vertices; i++)
    {
        delete[] distance_matrix[i];
        delete[] paths_matrix[i];
        delete[] intermediate_matrix[i];
    }

    delete[] distance_matrix;
    delete[] paths_matrix;
    delete[] intermediate_matrix;
    return true;
}

// void floydsAlgorithm(ifstream &matrix_file)
// {
// }

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: ./shortestpaths <filename>";
        return 1;
    }
    ifstream matrix_file(argv[1]);
    if (!matrix_file)
    {
        cerr << "Error: Cannot open file '" << argv[1] << "'.";
        return 1;
    }
    int succesful = findPaths(matrix_file);
    matrix_file.close();
    return succesful;
}