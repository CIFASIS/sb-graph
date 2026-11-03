import argparse
import json


def generate_json(number_of_air_conditioners: int) -> str:
    json_data = {
        "nodes": [
            {
                "id": 1,
                "interval": [[1, int(number_of_air_conditioners)]],
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
                        "defs": [2]
                    },
                    {
                        "id": "noise",
                        "exp": [[1, 0]],
                        "defs": [4]
                    }
                ]
            },
            {
                "id": 2,
                "interval": [[1, int(number_of_air_conditioners)]],
                "lhs": [
                    {
                        "id": "ev_1",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "on",
                        "exp": [[1, 0]],
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
                        "id": "tref",
                        "exp": [[1, 0]],
                        "defs": [3]
                    }
                ]
            },
            {
                "id": 3,
                "interval": [[1, int(number_of_air_conditioners)]],
                "lhs": [
                    {
                        "id": "ev_2",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "tref",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "nextTref",
                        "exp": [[1, 0]],
                        "defs": []
                    }

                ],
                "rhs": [
                ]
            },
            {
                "id": 4,
                "interval": [[1, int(number_of_air_conditioners)]],
                "lhs": [
                    {
                        "id": "ev_3",
                        "exp": [[1, 0]],
                        "defs": []
                    },
                    {
                        "id": "nextSample",
                        "exp": [[2, 0]],
                        "defs": []
                    },
                    {
                        "id": "noise",
                        "exp": [[1, 0]],
                        "defs": []
                    }

                ],
                "rhs": [
                ]
            }

        ]
    }

    return json.dumps(json_data)


def main():
    parser = argparse.ArgumentParser(description='Generate SBG JSON input from spiking neurons Modelica model.')
    parser.add_argument('size', type=int, help='The number of air conditioners population.')

    args = parser.parse_args()

    json_data = generate_json(args.size)
    print(json_data)

if __name__ == '__main__':
    main()