/*The goal of this program is to compare file extension type to the signature of the file and find mismatches*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip> // For std::setw and std::setfill
#include <sstream> // For std::stringstream
#include "FileBase.h"

using namespace std;

string trim(const string& str) {
  size_t first = str.find_first_not_of(" \t\n\r");
  size_t last = str.find_last_not_of(" \t\n\r");
  if (first == string::npos || last == string::npos) {
      return ""; // String is all whitespace
  }
  return str.substr(first, last - first + 1);
}

string getFileExtension(const string& filePath) {
    size_t lastDot = filePath.find_last_of('.');
    if (lastDot == string::npos) {
        return ""; // No extension found
    }
    cout << "File extension found: " << filePath.substr(lastDot + 1) << endl;
    return filePath.substr(lastDot + 1);
}

string getFileSignature(const string& filePath, size_t length) {
    ifstream file(filePath, ios::binary);
    if (!file) {
        cerr << "Error opening file: " << filePath << endl;
        return "";
    }

    // Calculate the number of bytes to read (length is in hex digits, so divide by 2)
    size_t bytesToRead = (length + 1) / 2; // Round up if length is odd

    // Check if the file is smaller than the requested number of bytes
    file.seekg(0, ios::end);
    size_t fileSize = file.tellg();
    if (fileSize < bytesToRead) {
        cerr << "File is smaller than the requested length." << endl;
        return "";
    }

    // Move the file pointer back to the beginning
    file.seekg(0, ios::beg);

    // Read the specified number of bytes from the file
    string signature(bytesToRead, '\0');
    file.read(&signature[0], bytesToRead);
    if (!file) {
        cerr << "Error reading file: " << filePath << endl;
        return "";
    }

    // Convert the bytes to a hex string
    stringstream hexSignature;
    for (unsigned char c : signature) {
        hexSignature << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(c);
    }

    // Close the file
    file.close();

    // Return the hex signature
    return hexSignature.str();
}

void LoadFileSignatures(RedBlackTree<string>& tree, const string& filePath) {
    ifstream file(filePath);
    if (!file) {
        cerr << "Error opening file: " << filePath << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        size_t firstComma = line.find(',');
        size_t secondComma = line.find(',', firstComma + 1);

        if (firstComma != string::npos && secondComma != string::npos) {
            // Extract the extension, file signature, and length
            string extension = line.substr(0, firstComma);
            string signature = line.substr(firstComma + 1, secondComma - firstComma - 1);
            size_t length = stoi(line.substr(secondComma + 1));

            // Insert the data into the Red-Black Tree
            tree.insert(signature, extension, length);
        } else {
            cerr << "Invalid line format: " << line << endl;
        }
    }

    file.close();
}

int main() {
    RedBlackTree<string> tree;
    LoadFileSignatures(tree, "FileSignature.txt");

    // Open the log file in append mode
    ofstream logFile("log.txt", ios::app);
    if (!logFile) {
        cerr << "Error opening log file." << endl;
        return 1;
    }

    string filePath;
    cout << "Enter the file path: ";
    cin >> filePath;

    // Log the file path
    logFile << "File path entered: " << filePath << endl;

    string extension = trim(getFileExtension(filePath));
    logFile << "Extracted file extension: " << extension << endl;

    bool isMatch = false; // The file signature to search for
    size_t length = 0;
    vector<string> signatures; // To store all associated extensions

    if (tree.search(extension, signatures, length)) {
        string fileSignature = getFileSignature(filePath, length);
        logFile << "Extracted file signature: " << fileSignature << endl;

        // Check if the file signature matches any in the database
        for (string element : signatures) {
            if (element == fileSignature) {
                isMatch = true;
                break;
            }
        }

        if (isMatch) {
            cout << "File signature matches the expected signature." << endl;
            logFile << "Result: File signature matches the expected signature." << endl;
        } else {
            cout << "File signature does not match the expected signature." << endl;
            logFile << "Result: Uh oh File signature does not match the expected signature." << endl;
            logFile << "                                .--.__\n"
                    << "                                                      .~ (@)  ~~~---_\n"
                    << "                                                     {     `-_~,,,,,,)\n"
                    << "                                                     {    (_  ',\n"
                    << "                                                      ~    . = _',\n"
                    << "                                                       ~-   '.  =-'\n"
                    << "                                                         ~     :\n"
                    << "      .                                             _,.-~     ('');\n"
                    << "      '.                                         .-~        \\  \\ ;\n"
                    << "        ':-_                                _.--~            \\  \\;      _-=,.\n"
                    << "          ~-:-.__                       _.-~                 {  '---- _'-=,.\n"
                    << "             ~-._~--._             __.-~                     ~---------=,.`\n"
                    << "                 ~~-._~~-----~~~~~~       .+++~~~~~~~~-__   /\\\n"
                    << "                      ~-.,____           {   -     +   }  _/\\\n"
                    << "                              ~~-.______{_    _ -=\\ / /_.~\\\n"
                    << "                                   :      ~--~    // /         ..-\\\n"
                    << "                                   :   / /      // /         ((\\\n"
                    << "                                   :  / /      {   `-------,. ))\n"
                    << "                                   :   /        ''=--------. }o\n"
                    << "                      .=._________,\\'  )                     ))\n"
                    << "                      )  _________ -''                     ~~\n"
                    << "                     / /  _ _\n"
                    << "                    (_.-.'O'-'.\n";
        }

        // Print all associated signatures
        cout << "All Associated signatures: ";
        logFile << "All Associated signatures: ";
        for (const auto& ext : signatures) {
            cout << ext << " ";
            logFile << ext << " ";
        }
        cout << endl;
        logFile << endl;

        cout << "Current signature: " << fileSignature << endl;
        logFile << "Current signature: " << fileSignature << endl;
    } else {
        cout << "No matching file signature found in the database." << endl;
        logFile << "Result: No matching file signature found in the database." << endl;
    }
    logFile << "----------------------------------------" << endl;
    // Close the log file
    logFile.close();

    return 0;
}


/*To do:
get file extension as well as hex signature and compare to data base. If hex code is found compare each extension type to current file extension and identify if it is mistmatched or not.*/

