import argparse
import json

def generate_section_controller_records(sections, airs_per_section, enumerate_sections, on_definitions):
    records = []
    # Generate list for partial total node definition
    for section_nbr in range(sections):
        record = {
            "id": int(10 + section_nbr),
            "interval": [[int(airs_per_section * section_nbr + 1), int(airs_per_section + airs_per_section * section_nbr)]],
            "lhs": [
                {
                    "id": "ev_" + str(7 + int(section_nbr)),
                    "exp": [[1, 0]],
                    "defs": []
                },
                {
                    "id": "on",
                    "exp": [[1, 0]],
                    "defs": []
                },
                {
                    "id": "partTotal",
                    "exp": [[0, int(section_nbr + 1)]],
                    "defs": []
                }
            ],
            "rhs": [
                {
                    "id": "th",
                    "exp": [[1, 0]],
                    "defs": [1]
                },
                {
                    "id": "on",
                    "exp": [[1, 0]],
                    "defs": on_definitions
                },
                {
                    "id": "dtref",
                    "exp": [[0, 1]],
                    "defs": [8]
                }
            ]
        }
        records.append(record)
    
    return json.dumps(records)

def generate_json(sections, size):
    """
    Generates a JSON file input for the air conditioners with controller Modelica model.

    Parameters:
    sections (int): The number of sections in which the air conditioners population is divided.
    size (int): Size of the air conditioners population.

    Returns:
    str: A JSON-formatted string for air conditioners with controller Modelica model used as SBG Partitioner input.
    """
    airs_per_section = size/sections
    enumerate_sections = [int(4)] + [int(10 + i) for i in range(sections)]
    on_definitions = [int(10 + i) for i in range(sections)]
    data = {
        "nodes": [
            {
                "id": 1,
                "interval": [[1, size]],
                "lhs": [
                    {
                        "id": "th",
                        "exp": [[1, 0]],
                        "defs": []
                    }
                ],
                "rhs": [
                    {
                        "id": "on",
                        "exp": [[1, 0]],
                        "defs": on_definitions
                    },
                    {
                        "id": "noise",
                        "exp": [[1, 0]],
                        "defs": [9]
                    },
                    {
                        "id": "th",
                        "exp": [[1, 0]],
                        "defs": [1]
                    }
                ]
            },
            {
                "id": 2,
                "interval": [[1, 1]],
                "lhs": [
                    {
                        "id": "ierr",
                        "exp": [[0, 1]],
                        "defs": []
                    }
                ],
                "rhs": [
                    {
                        "id": "pref",
                        "exp": [[0, 1]],
                        "defs": [6, 7]
                    },
                    {
                        "id": "ptotals",
                        "exp": [[0, 1]],
                        "defs": [8]
                    }
                ]
            },
            {
                "id": 3,
                "interval": [[1, 1]],
                "lhs": [
                    {
                        "id": "ptotal",
                        "exp": [[0, 1]],
                        "defs": []
                    }
                ],
                "rhs": []
            },
            {
                "id": 4,
                "interval": [[1, sections]],
                "lhs": [
                    {
                        "id": "ev_1",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "partSample",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "update",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "sendPartTotal",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "partTotal",
                        "exp": [[1, 0]],
                        "defs": []
                    }
                ],
                "rhs": [
                    {
                        "id": "partSample",
                        "exp": [[1, 0]],
                        "defs": [4]
                    },
                    {
                        "id": "partTotal",
                        "exp": [[1, 0]],
                        "defs": enumerate_sections
                    }
                ]
            },
            {
                "id": 5,
                "interval": [[1, sections]],
                "lhs": [
                    {
                        "id": "ev_2",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "ptotal",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "update",
                        "exp": [[1, 0]],
                        "defs": []
                    }
                ],
                "rhs": [
                    {
                        "id": "update",
                        "exp": [[1, 0]],
                        "defs": [4, 5]
                    },
                    {
                        "id": "ptotal",
                        "exp": [[1, 0]],
                        "defs": [3, 5]
                    },
                    {
                        "id": "sendPartTotal",
                        "exp": [[1, 0]],
                        "defs": [4]
                    }
                ]
            },
            {
                "id": 6,
                "interval": [[1, 1]],
                "lhs": [
                    {
                        "id": "ev_3",
                        "exp": [[0, 1]],
                        "defs": []
                    },
                    {
                        "id": "pref",
                        "exp": [[0, 1]],
                        "defs": []
                    }
                ],
                "rhs": []
            },
            {
                "id": 7,
                "interval": [[1, 1]],
                "lhs": [
                    {
                        "id": "ev_4",
                        "exp": [[0, 1]],
                        "defs": []
                    },
                    {
                        "id": "pref",
                        "exp": [[0, 1]],
                        "defs": []
                    }
                ],
                "rhs": []
            },
            {
                "id": 8,
                "interval": [[1, 1]],
                "lhs": [
                    {
                        "id": "ev_5",
                        "exp": [[0, 1]],
                        "defs": []
                    },
                    {
                        "id": "nextSample",
                        "exp": [[0, 1]],
                        "defs": []
                    },
                    {
                        "id": "ptotals",
                        "exp": [[0, 1]],
                        "defs": []
                    },
                    {
                        "id": "dtref",
                        "exp": [[0, 1]],
                        "defs": []
                    }
                ],
                "rhs": [
                    {
                        "id": "nextSample",
                        "exp": [[0, 1]],
                        "defs": [8]
                    },
                    {
                        "id": "ptotal",
                        "exp": [[0, 1]],
                        "defs": [3, 5]
                    },
                    {
                        "id": "ptotals",
                        "exp": [[0, 1]],
                        "defs": [8]
                    },
                    {
                        "id": "pref",
                        "exp": [[0, 1]],
                        "defs": [6, 7]
                    },
                    {
                        "id": "ierr",
                        "exp": [[0, 1]],
                        "defs": [2]
                    }
                ]
            },
            {
                "id": 9,
                "interval": [[1, size]],
                "lhs": [
                    {
                        "id": "ev_6",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "sampleNoise",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "inputs",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "noise",
                        "exp": [[1, 0]],
                        "defs": []
                    }
                ],
                "rhs": [
                    {
                        "id": "sampleNoise",
                        "exp": [[1, 0]],
                        "defs": [10]
                    },
                    {
                        "id": "inputs",
                        "exp": [[1, 0]],
                        "defs": [10]
                    }
                ]
            }
        ]
    }

    # Concatenate section controller records
    section_records = json.loads(generate_section_controller_records(sections, airs_per_section, enumerate_sections, on_definitions))
    data["nodes"].extend(section_records)

    return json.dumps(data)

def main():
    parser = argparse.ArgumentParser(description='Generate SBG JSON input from air conditioners with controller Modelica model.')
    parser.add_argument('sections', type=int, help='The number of sections for air conditioners controllers.')
    parser.add_argument('size', type=int, help='The size of the air conditioners population.')

    args = parser.parse_args()

    result = generate_json(args.sections, args.size)
    print(result)

if __name__ == '__main__':
    main()