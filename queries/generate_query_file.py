import argparse
import datetime
import os
import random

from datetime import timedelta
from random import randrange

def make_args_parser():
    # create an ArgumentParser object
    parser = argparse.ArgumentParser()
    # fill parser with information about program arguments
    parser.add_argument('--queryFile', default='queries.txt',
                         help='Name of the output query file', metavar='')
    parser.add_argument('--numQueries', default=20, type=int,
                         help='Number of queries to generate', metavar='')
    parser.add_argument('--commandsFile', default='../data/commands.txt',
                         help='File with command names', metavar='')
    parser.add_argument('--countriesFile', default='../data/countries.txt',
                         help='File with country names', metavar='')
    parser.add_argument('--diseasesFile', default='../data/diseases.txt',
                         help='File with disease names', metavar='')
    parser.add_argument('--maxK', default=4, type=int,
                        help='Max k-parameter', metavar='')
    parser.add_argument('--maxRecordID', default=1000, type=int,
                        help='Max RecordID', metavar='')
    parser.add_argument('--startDate', default='01-01-2020',
                        help='Start date', metavar='')
    parser.add_argument('--endDate', default='10-01-2020',
                        help='End date', metavar='')
    # return an ArgumentParser object
    return parser.parse_args()

def print_args(args):
    print("Running with the following configuration")
    # get the __dict__ attribute of args using vars() function
    args_map = vars(args)
    for key in args_map:
        print('\t', key, '-->', args_map[key])
    # add one more empty line for better output
    print()

def random_date(start_date, end_date):
    """
    This function will return a random datetime between two datetime
    objects.
    """
    time_between_dates = end_date - start_date
    days_between_dates = time_between_dates.days
    random_number_of_days = random.randrange(days_between_dates)
    random_date = start_date + datetime.timedelta(days=random_number_of_days)
    return random_date.strftime('%d-%m-%Y')

def main():
    # parse and print arguments
    args = make_args_parser()
    print_args(args)
    # Open files
    commands = open(args.commandsFile).read().splitlines()
    countries = open(args.countriesFile).read().splitlines()
    diseases = open(args.diseasesFile).read().splitlines()
    # Initiliaze datetime objects
    start_date = datetime.datetime.strptime(args.startDate, '%d-%m-%Y')
    end_date = datetime.datetime.strptime(args.endDate, '%d-%m-%Y')
    # Create queryFile
    with open(args.queryFile, "w") as query_file:
        # Generate numQueries random queries
        for i in range(args.numQueries):
            command = random.choice(commands)
            query = str(command.split()[0])
            query += " "
            for index, token in enumerate(command.split()):
                if index > 0:
                    if token == 'disease':
                        query += random.choice(diseases)
                        query += " "
                    elif token == '[country]':
                        flip = random.randint(0, 1)
                        if flip:
                            query += random.choice(countries)
                            query += " "
                    elif token == 'country':
                        query += random.choice(countries)
                        query += " "
                    elif token == 'k':
                        query += str(random.randint(1, args.maxK))
                        query += " "
                    elif token == 'recordID':
                        query += str(random.randint(0, args.maxRecordID))
                        query += " "
                    elif token == 'date1':
                        temp = str(random_date(start_date, end_date))
                        query += temp
                        query += " "
                        command_date1 = datetime.datetime.strptime(temp, '%d-%m-%Y')
                    elif token == 'date2':
                        query += str(random_date(command_date1, end_date))
                        query += " "
            query_file.write(query.rstrip())
            query_file.write("\n")

if __name__ == '__main__':
    main()
