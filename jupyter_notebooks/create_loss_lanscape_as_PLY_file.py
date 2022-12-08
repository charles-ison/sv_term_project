# -*- coding: utf-8 -*-

import torch
import torchvision
import torchvision.transforms as transforms
import matplotlib.pyplot as plt
import numpy as np
import torchvision.models as models
import torch.nn as nn
import torch.optim as optim
import copy
from datetime import datetime
import os
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA
from plyfile import PlyData, PlyElement
import csv

START_PCA_RANGE = -9
END_PCA_RANGE = 10
PCA_STEP_SIZE = 0.75

START_RANDOM_RANGE = -0.5
END_RANDOM_RANGE = 0.54
RANDOM_STEP_SIZE = 0.04

NUM_EPOCHS = 12
time_now = datetime.now()

OUTPUT_DIR = f"runs/{NUM_EPOCHS}_epochs-{time_now.hour}h_{time_now.minute}m_{time_now.month}_{time_now.day}_{time_now.year}"
os.mkdir(OUTPUT_DIR)


"""Functions for Exporting to PLY file:"""

def generate_vertex_data_from_arr(X, Y, Z):
    """Returns a PlyElement listing each vertex of data."""

    # Create a list of vertices.
    vertex_data_points = []

    for r_idx in range(len(Y)):
        for c_idx in range(len(Y[0])):
            z, vx, vy, vz = 0, 0.0, 0.0, 0.0
            
            x = X[r_idx][c_idx]
            y = Y[r_idx][c_idx]
            s = Z[r_idx][c_idx]
            vertex_data_points.append((x, y, z, vx, vy, vz, s))
        
    # Export the list of vertices.
    # numpy data types: ['int8', 'i1', 'char', 'uint8', 'u1', 'uchar', 'b1', 'int16', 'i2', 'short', 'uint16', 'u2', 'ushort', 'int32', 'i4', 'int', 'uint32', 'u4', 'uint', 'float32', 'f4', 'float', 'float64', 'f8', 'double']
    vertices = np.array(vertex_data_points,
                         dtype=[('x', 'float64'), ('y', 'float64'), ('z', 'float64'),
                                ('vx', 'float64'), ('vy', 'float64'), ('vz', 'float64'),
                                ('s', 'float64')])

    return PlyElement.describe(vertices, 'vertex')

def generate_face_data(num_rows):
    """Returns a PlyElement listing all of the faces of a square plane.
       Params:
            - num_rows = an int equal to the number of rows/cols of data, i.e. number of vertices in a direction.
    """
    row_size = num_rows - 1
    num_faces = row_size**2

    # For every face, insert it into `data` as a tuple, which we will turn into a PlyElement.
    data = []
    x = 0
    while x < num_faces + row_size:
        if x % row_size - (x // row_size - 1) == 0:
            x += 1
            continue

        next_row_offset = row_size + 2 + x
        data.append(([x, x + 1, next_row_offset, next_row_offset - 1]))
        x += 1

    face_data = np.array(data,
                            dtype=[('vertex_indices', 'int32', (4,))])

    # PlyElement.describe requires a one-dimensional structured array, so we have to do this in two lines.
    ply_faces = np.empty(len(face_data),
                            dtype=[('vertex_indices', 'int32', (4,))])
    ply_faces['vertex_indices'] = face_data

    element_2 = PlyElement.describe(ply_faces, 'face')
    return element_2

def output_ply(name, X, Y, Z):
    """Outputs a ply file for a square scalar field of vertices at (x = X[i], y = Y[i], s = Z[i])."""

    output_filename = f'{OUTPUT_DIR}/new_data_{name}.ply'

    # Generate the face and vertex elements.
    num_rows = len(X)
    vertex_element = generate_vertex_data_from_arr(X, Y, Z)
    face_element = generate_face_data(num_rows)

    # Create a PlyData object and write it to a file.
    new_ply_data = PlyData([vertex_element, face_element], text=True)
    new_ply_data.write(output_filename)

    print(f"Saved to {output_filename}")


"""
## Functions for Training the Model:
"""

def get_data():
    transform = transforms.Compose([
        transforms.Resize((224, 224)),
        transforms.ToTensor(),
        transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
    ])
    
    training_data = torchvision.datasets.CIFAR10(root = 'data',
                                               train = True,
                                               transform = transform,
                                               download = True)

    testing_data = torchvision.datasets.CIFAR10(root = 'data',
                                              train = False,
                                              transform = transform,
                                              download=True)
    
    return training_data, testing_data

def get_loaders(training_data, testing_data, batch_size):
    training_loader = torch.utils.data.DataLoader(dataset = training_data,
                                                  batch_size = batch_size,
                                                  shuffle = True)

    testing_loader = torch.utils.data.DataLoader(dataset = testing_data,
                                                 batch_size = batch_size,
                                                 shuffle = True)
    
    return training_loader, testing_loader

def train(model, training_loader, criterion, optimizer):
    model.train()
    running_loss = 0.0
    num_correct = 0
    for i, data in enumerate(training_loader):
        # TODO: remove this, just here to cut down on running time
        if(i == 100):
            break

        data, labels = data[0].to(device), data[1].to(device)
        optimizer.zero_grad()
        output = model(data)
        loss = criterion(output, labels)
        running_loss += loss.item()
        _, predictions = torch.max(output.data, 1)
        num_correct += (predictions == labels).sum().item()
        loss.backward()
        optimizer.step()
    
    # TODO: remove this, just here to cut down on running time
    loss = running_loss/(10*100)
    #loss = running_loss/len(training_loader.dataset)
    accuracy = num_correct/ (10*100)
    #accuracy = num_correct/len(training_loader.dataset)
    print(f'Train Loss: {loss:.4f}, Train Acc: {accuracy:.2f}')
    
    return loss, accuracy

def test(model, testing_loader, criterion):
    model.eval()
    running_loss = 0.0
    num_correct = 0
    for i, data in enumerate(testing_loader):
        # TODO: remove this, just here to cut down on running time
        if(i == 100):
            break

        data, labels = data[0].to(device), data[1].to(device)
        output = model(data)
        loss = criterion(output, labels)
        running_loss += loss.item()
        _, predictions = torch.max(output.data, 1)
        num_correct += (predictions == labels).sum().item()
    
    # TODO: remove this, just here to cut down on running time
    loss = running_loss/(10*100)
    #loss = running_loss/len(testing_loader.dataset)
    accuracy = num_correct/(10*100)
    #accuracy = num_correct/len(testing_loader.dataset)
    print(f'Test Loss: {loss:.4f}, Test Acc: {accuracy:.2f}')
    
    return loss, accuracy


def get_training_loss(model, training_loader, criterion):
    model.eval()
    running_loss = 0.0
    for i, data in enumerate(training_loader):
        # TODO: remove this, just here to cut down on running time
        if(i == 100):
            break

        data, labels = data[0].to(device), data[1].to(device)
        output = model(data)
        loss = criterion(output, labels)
        running_loss += loss.item()
    
    # TODO: remove this, just here to cut down on running time
    loss = running_loss/(10*100)
    #loss = running_loss/len(training_loader.dataset)
    print(f'Training Loss: {loss:.4f}')
    
    return loss

def pre_train_and_test(model, training_loader, testing_loader, criterion, optimizer):

    historical_weights = []
    historical_losses = []

    for epoch in range(NUM_EPOCHS):
        print("epoch: " + str(epoch))
        training_loss, training_accuracy = train(model, training_loader, criterion, optimizer)
        testing_loss, testing_accuracy = test(model, testing_loader, criterion)

        training_weights = get_weights(model)
        historical_weights.append(copy.deepcopy(training_weights))
        historical_losses.append(training_loss)

    return historical_weights[:-1], historical_losses[:-1]

def get_weights(model):
    return [param.data for param in model.parameters()]

def get_random_directions(weights, device):
    return [torch.randn(w.size()).to(device) for w in weights]

def normalize_directions(directions, weights):
    for d, w in zip(directions, weights):
        d.mul_(w.norm()/(d.norm() + 1e-10))

def tensorlist_to_tensor(weights):
    return torch.cat([w.cpu().view(w.cpu().numel()) if w.dim() > 1 else torch.FloatTensor(w.cpu()) for w in weights])

def get_diff_weights(final_weights, historical_weights):
    return [h2 - fw for (fw, h2) in zip(final_weights, historical_weights)]

def plot_figure(X, Y, Z, title):
    """Plot a temporary figure of the loss landscape to verify our results."""
    fig = plt.figure(figsize=(10,7.5))
    ax0 = fig.add_subplot(projection='3d' )

    ax0.view_init(elev=30, azim=-20)
    ax0.set_xlabel('X', labelpad=9)
    ax0.set_ylabel('Y', labelpad=-5)
    ax0.set_zlabel("Loss", labelpad=-30)
    ax0.tick_params(axis='x', pad=5, which='major')
    ax0.tick_params(axis='y', pad=-5, which='major')
    ax0.tick_params(axis='z', pad=5, which='major')
    ax0.set_title(title, y=0.85)
    ax0.plot_surface(X, Y, Z, cmap='terrain', antialiased=True, cstride=1, rstride=1, alpha=0.75)

    plt.savefig(f"{OUTPUT_DIR}/{title.replace(' ', '_')}.png")

def get_random_direction_losses(model, training_loader, testing_loader, criterion, device):
    
    weights = get_weights(model)

    random_x_directions = get_random_directions(weights, device)
    random_y_directions = get_random_directions(weights, device)

    normalize_directions(random_x_directions, weights)
    normalize_directions(random_y_directions, weights)

    x_coordinates = np.arange(START_RANDOM_RANGE, END_RANDOM_RANGE, RANDOM_STEP_SIZE)   
    y_coordinates = np.arange(START_RANDOM_RANGE, END_RANDOM_RANGE, RANDOM_STEP_SIZE)   
    X, Y = np.meshgrid(x_coordinates, y_coordinates)
    Z = np.zeros((x_coordinates.size, y_coordinates.size))

    for y_index, y in enumerate(y_coordinates):
        for x_index, x in enumerate(x_coordinates):
        
            print("X: " + str(x) + ", Y: " + str(y))
            updated_x_directions = [direction * x for direction in random_x_directions]
            updated_y_directions = [direction * y for direction in random_y_directions]
            delta = [sum(direction) for direction in zip(updated_x_directions, updated_y_directions)]
            old_weights = copy.deepcopy(get_weights(model))
        
            for (p, w, d) in zip(model.parameters(), old_weights, delta):
                p.data = w + torch.Tensor(d).type(type(w)).to(device)
            
            training_loss = get_training_loss(model, training_loader, criterion)
            Z[y_index][x_index] = training_loss
        
            for (p, w) in zip(model.parameters(), old_weights):
                p.data = w
                
    return X, Y, Z

def pca_to_directions(pca, weights):
      directions = copy.deepcopy(weights)
      idx = 0
      for w in directions:
          w.copy_(torch.tensor(pca[idx:idx + w.numel()]).view(w.size()))
          idx += w.numel()
      return directions

def write_gradient_descent_results_to_csv(weight_change_directions, pca_x_directions, pca_y_directions, title):
    pca_x_directions = tensorlist_to_tensor(pca_x_directions)
    pca_y_directions = tensorlist_to_tensor(pca_y_directions)
    with open(f'{title}_gradient_descent_results.csv', 'w', encoding='UTF8') as file:
        writter = csv.writer(file)
        for weight in zip(weight_change_directions):
            x = np.dot(weight, pca_x_directions)/pca_x_directions.norm()
            y = np.dot(weight, pca_y_directions)/pca_y_directions.norm()         
            writter.writerow([x.item(), y.item()])

def get_pca_losses(model, training_loader, criterion, device, historical_weights, title):

    weight_change_directions = []
    for weight in historical_weights:
      weight_change_direction = get_diff_weights(historical_weights[-1], weight)
      weight_change_directions.append(tensorlist_to_tensor(weight_change_direction).numpy())

    pca = PCA(n_components=2)
    weight_change_directions = np.array(weight_change_directions)
    pca.fit(weight_change_directions)
    pca1 = np.array(pca.components_[0])
    pca2 = np.array(pca.components_[1])

    pca_x_directions = pca_to_directions(pca1, historical_weights[-1])
    pca_y_directions = pca_to_directions(pca2, historical_weights[-1])
    write_gradient_descent_results_to_csv(weight_change_directions, pca_x_directions, pca_y_directions, title)

    x_coordinates = np.arange(START_PCA_RANGE, END_PCA_RANGE, PCA_STEP_SIZE)   
    y_coordinates = np.arange(START_PCA_RANGE, END_PCA_RANGE, PCA_STEP_SIZE)   

    X, Y = np.meshgrid(x_coordinates, y_coordinates)
    Z = np.zeros((x_coordinates.size, y_coordinates.size))

    for y_index, y in enumerate(y_coordinates):
        for x_index, x in enumerate(x_coordinates):

            print("X: " + str(x) + ", Y: " + str(y))
            updated_x_directions = [direction * x for direction in pca_x_directions]
            updated_y_directions = [direction * y for direction in pca_y_directions]
            delta = [sum(direction) for direction in zip(updated_x_directions, updated_y_directions)]
            old_weights = copy.deepcopy(get_weights(model))

            for (p, w, d) in zip(model.parameters(), old_weights, delta):
                p.data = w + torch.Tensor(d).type(type(w)).to(device)
            
            training_loss = get_training_loss(model, training_loader, criterion)
            Z[y_index][x_index] = training_loss
        
            for (p, w) in zip(model.parameters(), old_weights):
                p.data = w
        
    return X, Y, Z

def create_random_direction_loss_landscape(model, training_loader, testing_loader, criterion, device, title):
    X, Y, Z = get_random_direction_losses(model, training_loader, testing_loader, criterion, device)
    plot_figure(X, Y, Z, title)
    output_ply(f"{title}_random", X, Y, Z)

def create_pca_loss_landscape(model, training_loader, criterion, device, historical_weights, title):
    X, Y, Z = get_pca_losses(model, training_loader, criterion, device, historical_weights, title)
    plot_figure(X, Y, Z, title)
    output_ply(f"{title}_pca", X, Y, Z)

def create_loss_landscapes(model, training_loader, testing_loader, device, graph_title):
    model.to(device)
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.SGD(model.parameters(), lr=0.001, momentum=0.9)
    historical_weights, historical_losses = pre_train_and_test(model, training_loader, testing_loader, criterion, optimizer)
    create_pca_loss_landscape(model, training_loader, criterion, device, historical_weights, graph_title + "_PCA")
    create_random_direction_loss_landscape(model, training_loader, testing_loader, criterion, device, graph_title + "_Random_Directions")

def create_VGG_loss_landscapes(training_loader, testing_loader, device):
    vgg11 = models.vgg11(weights = models.VGG11_Weights.DEFAULT)
    
    # change the number of classes 
    vgg11.classifier[6].out_features = 10
    
    create_loss_landscapes(vgg11, training_loader, testing_loader, device, 'VGG')

def create_ResNet_loss_landscapes(training_loader, testing_loader, device):
    resnet50 = models.resnet50(weights = models.ResNet50_Weights.DEFAULT)

    # change the number of classes 
    resnet50.fc.out_features = 10
    
    create_loss_landscapes(resnet50, training_loader, testing_loader, device, 'ResNet')


print("Gathering data")
training_data, testing_data = get_data()
training_loader, testing_loader = get_loaders(training_data, testing_data, 10)
classes = training_data.classes

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
torch.cuda.empty_cache()

print("Creating loss landscape for VGG")
create_VGG_loss_landscapes(training_loader, testing_loader, device)

print("Creating loss landscape for ResNet")
create_ResNet_loss_landscapes(training_loader, testing_loader, device)
