import argparse
import json


def generate_json(elements: int) -> str:
    json_data = {
        "nodes": [
            {
                "id": 1,
                "interval": [[1, 1]],
                "lhs": [
                    {
                        "id": "X1",
                        "exp": [[0, 1]],
                        "defs": []
                    }
                ],
                "rhs": [
                    {
                        "id": "X1",
                        "exp": [[0, 1]],
                        "defs": [1, 2]
                    }
                ]
            },
            {
                "id": 2,
                "interval":[ [2, int(elements)]],
                "lhs": [
                    {
                        "id": "X1",
                        "exp": [[1, 0]],
                        "defs": []
                    }
                ],
                "rhs": [
                    {
                        "id": "X1",
                        "exp": [[1, 0]],
                        "defs": [1, 2]
                    },
                    {
                        "id": "X1",
                        "exp": [[1, -1]],
                        "defs": [1, 2]
                    }
                ]
            }
        ]
    }

    return json.dumps(json_data)


def main():
    parser = argparse.ArgumentParser(description='Generate SBG JSON input.')
    parser.add_argument('size', type=int, help='The number of elements.')

    args = parser.parse_args()

    json_data = generate_json(args.size)
    print(json_data)

if __name__ == '__main__':
    main()
