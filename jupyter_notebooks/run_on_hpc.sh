#!/bin/bash
#SBATCH -J create_loss_vis
#SBATCH -p cascades
#SBATCH -A cascades
#SBATCH --gres=gpu:1         
#SBATCH --mem=32G   
#SBATCH -c 16
#SBATCH --nodelist=cn-m-2    
#SBATCH -t 1-00:00:00       
#SBATCH --export=ALL
python create_loss_hpc.py
