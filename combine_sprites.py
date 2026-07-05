"""
Script para combinar sprites individuais em uma spritesheet.
Funciona para qualquer número de frames — o grid é calculado automaticamente
ou pode ser especificado manualmente.

Uso:
    python combine_sprites.py
    python combine_sprites.py --input "recursos/img/meus_frames" --output "recursos/img/Spritesheet.png"
    python combine_sprites.py --input "recursos/img/meus_frames" --cols 4 --rows 3
"""
from PIL import Image
import os
import glob
import re
import argparse


def numeric_sort_key(filename):
    """Extrai números do nome do arquivo para ordenação numérica."""
    nums = re.findall(r'\d+', os.path.basename(filename))
    return int(nums[0]) if nums else 0


def auto_grid(num_frames):
    """
    Calcula automaticamente o grid mais quadrado possível.
    Ex: 8 frames -> 4x2, 10 frames -> 5x2, 12 frames -> 4x3, 5 frames -> 5x1
    """
    if num_frames <= 0:
        return 1, 1

    best_cols = num_frames
    best_rows = 1

    for cols in range(1, num_frames + 1):
        rows = (num_frames + cols - 1) // cols  # ceiling division
        if cols >= rows and cols * rows >= num_frames:
            if cols * rows < best_cols * best_rows:
                best_cols = cols
                best_rows = rows

    return best_cols, best_rows


def combine_sprites(input_dir, output_path, cols=None, rows=None):
    # Listar PNGs
    files = sorted(glob.glob(os.path.join(input_dir, "*.png")))
    files.sort(key=numeric_sort_key)

    if not files:
        print(f"❌ Nenhum arquivo .png encontrado em: {input_dir}")
        return False

    num_frames = len(files)

    # Auto calcular grid se não especificado
    if cols is None or rows is None:
        cols, rows = auto_grid(num_frames)

    # Verificar se cabe
    if cols * rows < num_frames:
        print(f"⚠️  Grid {cols}x{rows} ({cols*rows} slots) é menor que {num_frames} frames.")
        print(f"   Ajustando para {num_frames}x1...")
        cols = num_frames
        rows = 1

    # Abrir imagens e determinar tamanho máximo dos frames
    images = []
    max_w, max_h = 0, 0
    for f in files:
        img = Image.open(f)
        images.append(img)
        max_w = max(max_w, img.width)
        max_h = max(max_h, img.height)

    sheet_w = max_w * cols
    sheet_h = max_h * rows

    # Criar spritesheet com fundo transparente
    sheet = Image.new("RGBA", (sheet_w, sheet_h), (0, 0, 0, 0))

    print(f"\n{'='*50}")
    print(f"📁 Pasta:     {input_dir}")
    print(f"📦 Frames:    {num_frames}")
    print(f"📐 Grid:      {cols} colunas × {rows} linhas")
    print(f"🖼️  Tamanho frame: {max_w}×{max_h}")
    print(f"📄 Spritesheet:   {sheet_w}×{sheet_h}")
    print(f"{'='*50}\n")

    for i, img in enumerate(images):
        col = i % cols
        row = i // cols
        x = col * max_w
        y = row * max_h

        # Centralizar o frame no slot se ele for menor
        ox = (max_w - img.width) // 2
        oy = (max_h - img.height) // 2

        # Preservar canal alpha se existir
        if img.mode == "RGBA":
            sheet.paste(img, (x + ox, y + oy), img)
        else:
            sheet.paste(img, (x + ox, y + oy))

        print(f"  Frame {i:3d} → ({col}, {row})  pos=({x}, {y})  {img.filename}")

    sheet.save(output_path)
    print(f"\n✅ Spritesheet salva: {output_path}")
    return True


def main():
    parser = argparse.ArgumentParser(
        description="Combina sprites individuais em uma spritesheet."
    )
    parser.add_argument(
        "--input", "-i",
        default="recursos/img/player_frames",
        help="Pasta com os PNGs individuais (default: recursos/img/player_frames)"
    )
    parser.add_argument(
        "--output", "-o",
        default="recursos/img/Player.png",
        help="Arquivo de saída (default: recursos/img/Player.png)"
    )
    parser.add_argument(
        "--cols", "-c",
        type=int,
        default=None,
        help="Número de colunas (default: auto)"
    )
    parser.add_argument(
        "--rows", "-r",
        type=int,
        default=None,
        help="Número de linhas (default: auto)"
    )
    parser.add_argument(
        "--list", "-l",
        action="store_true",
        help="Apenas listar os arquivos encontrados, sem gerar a spritesheet"
    )

    args = parser.parse_args()

    # Verificar se a pasta existe
    if not os.path.isdir(args.input):
        print(f"❌ Pasta não encontrada: {args.input}")
        print(f"   Crie a pasta e coloque os PNGs lá dentro.")
        return

    files = sorted(glob.glob(os.path.join(args.input, "*.png")))
    files.sort(key=numeric_sort_key)

    if not files:
        print(f"❌ Nenhum arquivo .png encontrado em: {args.input}")
        return

    print(f"\n📂 {len(files)} arquivo(s) encontrado(s) em '{args.input}':")
    for f in files:
        img = Image.open(f)
        print(f"   {os.path.basename(f):30s} {img.size[0]}×{img.size[1]}  {img.mode}")

    if args.list:
        return

    # Confirmar se quer prosseguir
    print()
    try:
        resp = input("Criar spritesheet? (S/n): ").strip().lower()
        if resp == "n":
            print("Cancelado.")
            return
    except (EOFError, KeyboardInterrupt):
        pass  # em modo não-interativo, prossegue

    combine_sprites(args.input, args.output, args.cols, args.rows)


if __name__ == "__main__":
    main()
