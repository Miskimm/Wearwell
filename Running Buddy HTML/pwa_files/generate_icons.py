#!/usr/bin/env python3
"""
Generate PWA icons for Running Buddy app
"""

from PIL import Image, ImageDraw, ImageFont
import os

def create_icon(size):
    """Create a gradient icon with running person"""
    # create image
    img = Image.new('RGB', (size, size), 'white')
    draw = ImageDraw.Draw(img)
    
    # gradient background
    for y in range(size):
        r = int(102 + (118 - 102) * y / size)
        g = int(126 + (75 - 126) * y / size)
        b = int(234 + (162 - 234) * y / size)
        draw.rectangle([(0, y), (size, y+1)], fill=(r, g, b))
    
    # simple runner icon using shapes
    center_x = size // 2
    center_y = size // 2
    
    # head
    head_size = size // 6
    draw.ellipse(
        [(center_x - head_size//2, center_y - size//3 - head_size//2),
         (center_x + head_size//2, center_y - size//3 + head_size//2)],
        fill='white'
    )
    
    # body (simplified stick figure)
    line_width = max(size // 30, 3)
    
    # torso
    draw.line(
        [(center_x, center_y - size//6),
         (center_x + size//8, center_y + size//8)],
        fill='white',
        width=line_width
    )
    
    # left arm (forward)
    draw.line(
        [(center_x, center_y - size//8),
         (center_x - size//6, center_y)],
        fill='white',
        width=line_width
    )
    
    # right arm (backward)
    draw.line(
        [(center_x, center_y - size//8),
         (center_x + size//5, center_y - size//6)],
        fill='white',
        width=line_width
    )
    
    # left leg (forward)
    draw.line(
        [(center_x + size//8, center_y + size//8),
         (center_x, center_y + size//3)],
        fill='white',
        width=line_width
    )
    
    # right leg (backward)
    draw.line(
        [(center_x + size//8, center_y + size//8),
         (center_x + size//4, center_y + size//4)],
        fill='white',
        width=line_width
    )
    
    return img

def main():
    """Generate icons in different sizes"""
    sizes = [192, 512]
    assets_dir = 'assets'
    
    # create assets directory if it doesn't exist
    if not os.path.exists(assets_dir):
        os.makedirs(assets_dir)
    
    for size in sizes:
        print(f'Generating {size}x{size} icon...')
        icon = create_icon(size)
        icon.save(os.path.join(assets_dir, f'icon-{size}.png'), 'PNG')
        print(f'✓ Saved icon-{size}.png')
    
    print('\n✅ All icons generated successfully!')
    print(f'Icons saved to: {os.path.abspath(assets_dir)}/')

if __name__ == '__main__':
    main()

