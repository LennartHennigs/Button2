#!/bin/bash

# Button2 Library Compilation Test Script
# Tests compilation across multiple Arduino platforms and all examples
# Author: Generated for Button2 v2.x
# Date: October 25, 2024

set -e  # Exit on any error

#######################################################################
# CONFIGURATION CONSTANTS
#######################################################################

# Test platforms configuration
# Format: "FQBN|Display Name|PIO Board ID"
declare -a PLATFORMS=(
    "esp8266:esp8266:d1_mini|Wemos D1 Mini (ESP8266)|d1_mini"
    "esp32:esp32:m5stack_core2|M5Stack Core2 (ESP32)|m5stack-core-esp32"
    "arduino:avr:nano|Arduino Nano|nanoatmega328"
)

# Platform-specific example exclusions
# Format: "FQBN:example1,example2,..."
PLATFORM_EXCLUSIONS=(
    "arduino:avr:nano:ESP32CapacitiveTouch,ESP32ClassicCapacitiveTouch,ESP32S2S3CapacitiveTouch,ESP32TimerInterrupt,M5StackCore2CustomHandler"  # ESP32-only and M5Stack-only examples
    "esp8266:esp8266:d1_mini:ESP32CapacitiveTouch,ESP32ClassicCapacitiveTouch,ESP32S2S3CapacitiveTouch,ESP32TimerInterrupt,M5StackCore2CustomHandler"  # ESP32-only and M5Stack-only examples
    "esp32:esp32:m5stack_core2:ESP32CapacitiveTouch,ESP32ClassicCapacitiveTouch,ESP32S2S3CapacitiveTouch,ESP32TimerInterrupt"  # Non-M5Stack capacitive touch examples (M5Stack Core2 has different touch API)
)

# Examples directory
EXAMPLES_DIR="../examples"

#######################################################################
# DISPLAY CONFIGURATION
#######################################################################

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Get all example directories
EXAMPLES=($(find $EXAMPLES_DIR -name "*.ino" -exec dirname {} \; | sort | uniq))

# Function to print colored output
print_status() {
    local status=$1
    local message=$2
    case $status in
        "INFO")
            echo -e "${BLUE}[INFO]${NC} $message"
            ;;
        "SUCCESS")
            echo -e "${GREEN}[PASS]${NC} $message"
            ;;
        "ERROR")
            echo -e "${RED}[FAIL]${NC} $message"
            ;;
        "WARNING")
            echo -e "${YELLOW}[WARN]${NC} $message"
            ;;
    esac
}

# Function to test compilation
test_compilation() {
    local platform_fqbn=$1
    local platform_name=$2
    local pio_env=$3
    local example_path=$4
    local use_tool=$5
    local example_name=$(basename "$example_path")
    local ino_file="$example_path/$example_name.ino"
    
    # Check for platform-specific exclusions
    for exclusion in "${PLATFORM_EXCLUSIONS[@]}"; do
        local excl_platform=$(echo "$exclusion" | cut -d':' -f1-3)
        local excl_examples=$(echo "$exclusion" | cut -d':' -f4-)
        
        if [[ "$platform_fqbn" == "$excl_platform" ]] && [[ "$excl_examples" == *"$example_name"* ]]; then
            echo -e "  $example_name ${YELLOW}[SKIP]${NC} (platform-specific)"
            return 0
        fi
    done
    
    # Check if .ino file exists
    if [ ! -f "$ino_file" ]; then
        echo -e "  $example_name ${YELLOW}[SKIP]${NC} (file not found)"
        return 0
    fi
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # Compile with minimal output
    echo -n "  $example_name "
    
    if [[ "$use_tool" == "pio" ]]; then
        # Use PlatformIO CI
        local lib_path=""
        if [[ "$example_path" == *"examples/"* ]]; then
            lib_path="../../"  # From examples/ExampleName/ to library root
        else
            lib_path="../../../"  # Fallback path
        fi
        
        # Create minimal platformio.ini for dependencies
        local temp_ini="$example_path/platformio_temp.ini"
        local platform_name=""
        case "$pio_env" in
            d1_mini) platform_name="espressif8266" ;;
            m5stack-core-esp32) platform_name="espressif32" ;;
            nanoatmega328) platform_name="atmelavr" ;;
        esac
        
        # Add M5Stack dependencies for M5Stack examples
        if [[ "$example_name" == "M5StackCore2CustomHandler" ]]; then
            cat > "$temp_ini" << EOF
[env:test]
platform = $platform_name
board = $pio_env
framework = arduino
lib_deps =
    m5stack/M5Core2@^0.1.7
EOF
        else
            cat > "$temp_ini" << EOF
[env:test]
platform = $platform_name
board = $pio_env
framework = arduino
EOF
        fi
        
        if (cd "$example_path" && pio ci --project-conf="platformio_temp.ini" --lib="$lib_path" "$example_name.ino" > /dev/null 2>&1); then
            echo -e "${GREEN}[PASS]${NC}"
            PASSED_TESTS=$((PASSED_TESTS + 1))
            # Clean up temporary file
            rm -f "$temp_ini"
            return 0
        else
            echo -e "${RED}[FAIL]${NC}"
            FAILED_TESTS=$((FAILED_TESTS + 1))
            
            # Show compilation error for debugging
            echo -e "    ${RED}Error details:${NC}"
            (cd "$example_path" && pio ci --project-conf="platformio_temp.ini" --lib="$lib_path" "$example_name.ino" 2>&1 | tail -5 | sed 's/^/     /')
            echo ""
            # Clean up temporary file
            rm -f "$temp_ini"
            return 1
        fi
    else
        # Use Arduino CLI
        if arduino-cli compile --fqbn "$platform_fqbn" "$ino_file" --output-dir "/tmp/arduino-build-$example_name-$(date +%s)" > /dev/null 2>&1; then
            echo -e "${GREEN}[PASS]${NC}"
            PASSED_TESTS=$((PASSED_TESTS + 1))
            return 0
        else
            echo -e "${RED}[FAIL]${NC}"
            FAILED_TESTS=$((FAILED_TESTS + 1))
            
            # Show compilation error for debugging
            echo -e "    ${RED}Error details:${NC}"
            arduino-cli compile --fqbn "$platform_fqbn" "$ino_file" 2>&1 | tail -5 | sed 's/^/     /'
            echo ""
            return 1
        fi
    fi
}

# Function to check prerequisites
check_prerequisites() {
    local use_tool=$1
    print_status "INFO" "Checking prerequisites..."
    
    if [[ "$use_tool" == "pio" ]]; then
        if ! command -v pio &> /dev/null; then
            print_status "ERROR" "PlatformIO (pio) is not installed or not in PATH"
            echo "Please install PlatformIO: https://platformio.org/install/cli"
            exit 1
        fi
        
        print_status "SUCCESS" "PlatformIO found: $(pio --version | head -1)"
    else
        if ! command -v arduino-cli &> /dev/null; then
            print_status "ERROR" "arduino-cli is not installed or not in PATH"
            echo "Please install arduino-cli: https://arduino.github.io/arduino-cli/"
            exit 1
        fi
        
        print_status "SUCCESS" "arduino-cli found: $(arduino-cli version | head -1)"
    fi
    echo ""
}

# Function to check installed cores (only for Arduino CLI)
check_cores() {
    local use_tool=$1
    
    if [[ "$use_tool" != "pio" ]]; then
        print_status "INFO" "Checking required Arduino cores..."
        
        local cores_output=$(arduino-cli core list)
        
        # Check if required cores are installed
        local missing_cores=()
        
        if ! echo "$cores_output" | grep -q "esp8266:esp8266"; then
            missing_cores+=("esp8266:esp8266")
        fi
        
        if ! echo "$cores_output" | grep -q "esp32:esp32"; then
            missing_cores+=("esp32:esp32")
        fi
        
        if ! echo "$cores_output" | grep -q "arduino:avr"; then
            missing_cores+=("arduino:avr")
        fi
        
        if [ ${#missing_cores[@]} -gt 0 ]; then
            print_status "WARNING" "Missing cores detected: ${missing_cores[*]}"
            print_status "INFO" "Install missing cores with:"
            for core in "${missing_cores[@]}"; do
                echo "  arduino-cli core install $core"
            done
            echo ""
        else
            print_status "SUCCESS" "All required cores are installed"
            echo ""
        fi
    fi
}

# Function to run all tests
run_all_tests() {
    local selected_platform="$1"
    local use_tool="$2"
    
    print_status "INFO" "Starting compilation tests for Button2 library"
    echo "======================================================="
    echo ""
    
    # Test each platform with each example
    for platform_info in "${PLATFORMS[@]}"; do
        IFS='|' read -r platform_fqbn platform_name pio_env <<< "$platform_info"
        
        # Skip if specific platform requested and this isn't it
        if [[ -n "$selected_platform" ]]; then
            # Extract the board name from FQBN (last part after last colon)
            local board_name=$(echo "$platform_fqbn" | rev | cut -d':' -f1 | rev)
            
            # Check if selected platform matches either board name, display name, or pio env
            if [[ "$selected_platform" != "$board_name" ]] && 
               [[ "$platform_name" != *"$selected_platform"* ]] &&
               [[ "$platform_fqbn" != *"$selected_platform"* ]] &&
               [[ "$pio_env" != *"$selected_platform"* ]]; then
                continue
            fi
        fi
        
        print_status "INFO" "Testing platform: $platform_name (using $use_tool)"
        echo "----------------------------------------"
        
        for example_path in "${EXAMPLES[@]}"; do
            test_compilation "$platform_fqbn" "$platform_name" "$pio_env" "$example_path" "$use_tool"
        done
        
        echo ""
    done
}

# Function to display summary
show_summary() {
    local platform="$1"
    
    echo ""
    echo "======================================================="
    print_status "INFO" "Compilation Test Summary"
    echo "======================================================="
    
    if [[ -n "$platform" ]]; then
        echo "Platform:     $platform"
    else
        echo "Platform:     All platforms"
    fi
    
    echo "Total Tests:  $TOTAL_TESTS"
    echo -e "Passed:       ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed:       ${RED}$FAILED_TESTS${NC}"
    
    if [ $TOTAL_TESTS -eq 0 ]; then
        echo ""
        print_status "WARNING" "No tests were executed!"
        if [[ -n "$platform" ]]; then
            echo "The specified platform '$platform' was not found."
            echo "Use --help to see available platform options."
        fi
        exit 1
    elif [ $FAILED_TESTS -eq 0 ]; then
        echo ""
        print_status "SUCCESS" "All compilation tests passed! 🎉"
        if [[ -n "$platform" ]]; then
            echo "The Button2 library is compatible with the tested platform."
        else
            echo "The Button2 library is compatible with all tested platforms."
        fi
    else
        echo ""
        print_status "ERROR" "Some compilation tests failed!"
        echo "Please review the error messages above and fix any issues."
        exit 1
    fi
}

# Function to display help
show_help() {
    echo "Button2 Library Compilation Test Script"
    echo ""
    echo "Usage: $0 [OPTIONS] [PLATFORM]"
    echo ""
    echo "Options:"
    echo "  -h, --help         Show this help message"
    echo "  -v, --verbose      Enable verbose output"
    echo "  -q, --quiet        Minimal output (errors only)"
    echo "  --tool=TOOL        Use arduino-cli or pio (default: pio)"
    echo ""
    echo "Platform (optional):"
    echo "  If specified, tests only the selected platform. Can be:"
    echo "  - Board name: d1_mini, m5stack_core2, nano"
    echo "  - Display name: Wemos, M5Stack, Nano"
    echo "  - FQBN substring: esp8266, esp32, avr"
    echo "  - PIO environment: d1_mini, m5stack-core-esp32, nanoatmega328"
    echo ""
    echo "Available platforms:"
    echo "  - Wemos D1 Mini (ESP8266) - use: Wemos, d1_mini, or esp8266"
    echo "  - M5Stack Core2 (ESP32) - use: M5Stack, m5stack_core2, or esp32"
    echo "  - Arduino Nano (AVR) - use: Nano, nano, or avr"
    echo ""
    echo "Examples:"
    echo "  $0                        # Test all platforms with PlatformIO"
    echo "  $0 --tool=arduino-cli    # Test all platforms with Arduino CLI"
    echo "  $0 Wemos                # Test only Wemos D1 Mini with PlatformIO"
    echo "  $0 --tool=arduino-cli esp32  # Test only ESP32 with Arduino CLI"
    echo "  $0 -v nano              # Test Arduino Nano with verbose output"
    echo ""
    echo "Prerequisites:"
    echo "  - PlatformIO (pio) or arduino-cli must be installed and in PATH"
    echo "  - For Arduino CLI: Required Arduino cores must be installed"
    echo "  - For PlatformIO: Platforms will be installed automatically"
    echo ""
    echo "Platform-specific exclusions:"
    echo "  - ESP32CapacitiveTouch: ESP32 only"
    echo "  - ESP32ClassicCapacitiveTouch: ESP32 only"
    echo "  - ESP32S2S3CapacitiveTouch: ESP32 only"
    echo "  - ESP32TimerInterrupt: ESP32 only"
    echo "  - M5StackCore2CustomHandler: M5Stack Core2 only"
    echo ""
}

# Main execution
main() {
    local verbose_mode=""
    local quiet_mode=""
    local platform=""
    local use_tool="pio"  # Default to PlatformIO
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -v|--verbose)
                verbose_mode="1"
                set -x
                shift
                ;;
            -q|--quiet)
                quiet_mode="1"
                exec > /dev/null 2>&1
                shift
                ;;
            --tool=*)
                use_tool="${1#*=}"
                if [[ "$use_tool" != "pio" && "$use_tool" != "arduino-cli" ]]; then
                    print_status "ERROR" "Invalid tool: $use_tool. Must be 'pio' or 'arduino-cli'"
                    exit 1
                fi
                shift
                ;;
            -*)
                print_status "ERROR" "Unknown option: $1"
                show_help
                exit 1
                ;;
            *)
                if [[ -z "$platform" ]]; then
                    platform="$1"
                else
                    print_status "ERROR" "Multiple platforms specified: $platform and $1"
                    show_help
                    exit 1
                fi
                shift
                ;;
        esac
    done
    
    # Change to script directory
    cd "$(dirname "$0")"
    
    # Show configuration
    print_status "INFO" "Using build tool: $use_tool"
    if [[ -n "$platform" ]]; then
        print_status "INFO" "Testing selected platform: $platform"
    fi
    echo ""
    
    # Run the test suite
    check_prerequisites "$use_tool"
    check_cores "$use_tool"
    run_all_tests "$platform" "$use_tool"
    show_summary "$platform"
}

# Execute main function with all arguments
main "$@"
