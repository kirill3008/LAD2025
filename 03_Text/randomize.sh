#!/bin/sh

# Default delay is 0
delay=0
if [ $# -ge 1 ]; then
    if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
        echo "Usage: $0 [DELAY]"
        echo "Displays ASCII art character by character in random order"
        echo ""
        echo "  DELAY    Optional delay in seconds between characters"
        exit 0
    else
        delay="$1"
    fi
fi

# Create temporary files
input_file=$(mktemp)
output_file=$(mktemp)
trapped_files="$input_file $output_file"
trap 'rm -f $trapped_files' EXIT

# Read the entire input
cat > "$input_file"

# Create a normalized UTF-8 version of the input
normalized_file=$(mktemp)
trapped_files="$trapped_files $normalized_file"
# Try to normalize if iconv is available
cat "$input_file" > "$normalized_file"

# Count lines in input file
lines=$(wc -l < "$input_file")
max_width=0

# Determine max width by checking each line
while IFS= read -r line; do
    # Count characters, not bytes
    width=$(echo "$line" | wc -m)
    if [ "$width" -gt "$max_width" ]; then
        max_width="$width"
    fi
done < "$input_file"

# Get terminal dimensions for centering
if [ -t 1 ]; then
    term_size=$(stty size < /dev/tty 2>/dev/null || echo "24 80")
    term_rows=$(echo "$term_size" | cut -d' ' -f1)
    term_cols=$(echo "$term_size" | cut -d' ' -f2)
    
    row_offset=$(( (term_rows - lines) / 2 ))
    col_offset=$(( (term_cols - max_width) / 2 ))
    
    # Ensure offsets are not negative
    if [ "$row_offset" -lt 0 ]; then
        row_offset=0
    fi
    if [ "$col_offset" -lt 0 ]; then
        col_offset=0
    fi
else
    row_offset=0
    col_offset=0
fi

# Create a matrix of the input file with one character per line
# Format: row col char
row_num=0
while IFS= read -r line; do
    row_num=$((row_num + 1))
    col_num=0
    encoded_file=$(mktemp)
    char_file=$(mktemp)
    trapped_files="$trapped_files $encoded_file $char_file"
    echo -n "$line" | iconv -f UTF-8 -t UTF-32BE 2>/dev/null > "$encoded_file"
    encoded_size=$(wc -c < "$encoded_file")
    char_count=$((encoded_size / 4))
    
    col=1
    for i in $(seq 0 4 $((encoded_size - 1))); do
        if [ $((i + 4)) -le "$encoded_size" ]; then
            dd if="$encoded_file" bs=1 skip=$i count=4 2>/dev/null | \
               iconv -f UTF-32BE -t UTF-8 2>/dev/null > "$char_file"
        else
            break
        fi
        
        if [ -s "$char_file" ]; then
            # Read the resulting character
            char=$(cat "$char_file" 2>/dev/null)
            # Check if character is not empty and not a null byte
            if [ -n "$char" ] && [ "$(printf "%s" "$char" | wc -c)" -gt 0 ]; then
                echo "$row_num $col $char" >> "$output_file"
                col=$((col + 1))
            fi
        fi
    done
done < "$input_file"

clear

randomized_file=$(mktemp)
trapped_files="$trapped_files $randomized_file"

random_keys_file=$(mktemp)
trapped_files="$trapped_files $random_keys_file"

while read -r row col char; do
    now=$(date +%s%N 2>/dev/null || date +%s)
    r1=$(echo "$now $row $col" | cksum | cut -d' ' -f1)
    r2=$(echo "$r1$now" | cksum | cut -d' ' -f1)
    echo "$r1$r2 $row $col $char"
done < "$output_file" | sort -n | cut -d' ' -f2- > "$randomized_file"

while read -r row col char; do
    # Calculate position with offsets
    pos_row=$((row + row_offset))
    pos_col=$((col + col_offset))
    
    printf "\033[%d;%dH%s" "$pos_row" "$pos_col" "$char"
    
    if [ "$delay" != "0" ]; then
        sleep "$delay"
    fi
done < "$randomized_file"

printf "\033[%d;1H\n" "$((lines + row_offset + 1))"
