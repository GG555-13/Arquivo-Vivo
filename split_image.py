"""
Script para dividir uma imagem muito larga em N partes verticais.
Cada parte é salva como um PNG separado.
"""
from PIL import Image
import os

# CONFIGURAÇÕES
INPUT_PATH = "recursos/img/cidadearquivovivo.png"
OUTPUT_DIR = "recursos/img/"
NUM_PARTS = 3  # Quantas partes dividir

def split_image(input_path, output_dir, num_parts):
    img = Image.open(input_path)
    width, height = img.size
    
    print(f"Imagem original: {width}x{height} pixels")
    print(f"Cada parte terá aproximadamente: {width // num_parts}x{height} pixels")
    print()
    
    part_width = width // num_parts
    base_name = os.path.splitext(os.path.basename(input_path))[0]
    
    for i in range(num_parts):
        left = i * part_width
        # Última parte pega o resto para não perder pixels
        if i == num_parts - 1:
            right = width
        else:
            right = (i + 1) * part_width
        
        crop = img.crop((left, 0, right, height))
        output_name = f"{base_name}_parte{i+1}.png"
        output_path = os.path.join(output_dir, output_name)
        crop.save(output_path)
        
        print(f"  Parte {i+1}: {output_name} -> {crop.size[0]}x{crop.size[1]} pixels")
    
    print()
    print("Divisão concluída!")

if __name__ == "__main__":
    split_image(INPUT_PATH, OUTPUT_DIR, NUM_PARTS)
