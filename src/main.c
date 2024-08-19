/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekrause <emeric.yukii@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 14:20:17 by ekrause           #+#    #+#             */
/*   Updated: 2024/08/19 14:27:27 by ekrause          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"
#include <string.h>

void	ft_exit(char *error)
{
	printf(RED "%s\n" END, error);
	exit(1);
}

int	is_numeric(char **argv)
{
	int	i;
	int	j;

	i = 1;
	while (argv[i])
	{
		j = 0;
		while (argv[i][j])
		{
			if (argv[i][j] < '0' || argv[i][j] > '9' || strlen(argv[i]) > 9)
				return (0);
			j++;
		}
		i++;
	}
	return (1);
}

int	parse_arg(int argc, char **argv, t_arg **arg)
{
	if ((argc == 5 || argc == 6) && is_numeric(argv))
	{
		(*arg)->nb_philo = atoi(argv[1]);
		(*arg)->time_to_die = atoi(argv[2]);
		(*arg)->time_to_eat = atoi(argv[3]);
		(*arg)->time_to_sleep = atoi(argv[4]);
		if (argc == 6)
			(*arg)->must_eat = atoi(argv[5]);
		else
			(*arg)->must_eat = -1;
		return (1);
	}
	return (0);
}

void	init_philos_and_forks(t_arg *arg)
{
	int	i;

	arg->philos = malloc(sizeof(t_philo) * arg->nb_philo);
	arg->forks = malloc(sizeof(t_fork) * arg->nb_philo);
	if (!arg->philos || !arg->forks)
		return ;
	i = 0;
	while (i < arg->nb_philo)
	{
		arg->philos[i].id = i;
		arg->forks[i].id = i;
		arg->philos[i].last_meal = 0;
		arg->philos[i].nbr_meals = 0;
		arg->philos[i].left_fork = &arg->forks[i];
		arg->philos[i].right_fork = &arg->forks[(i + 1) % arg->nb_philo];
		pthread_mutex_init(&arg->forks[i].fork, NULL);
		i++;
	}
}

int is_end_of_simulation(t_philo *philo)
{
	if (philo->last_meal > philo->arg->time_to_die)
		ft_exit("philo died");
	if (philo->nbr_meals == philo->arg->must_eat)
		return (1);
	return (0);
}

void	*philo_routine(void *data)
{
	t_philo *philo = (t_philo*)data;
	t_arg	*arg = philo->arg;
	if (philo->id % 2 == 0)
		usleep(1);
	while (!is_end_of_simulation(philo))
	{
		printf("%d is thinking\n", philo->id);

		pthread_mutex_lock(&philo->left_fork->fork);
		printf("Philo %d took fork nb %d\n", philo->id, philo->left_fork->id);
		pthread_mutex_lock(&philo->right_fork->fork);
		printf("Philo %d took fork nb %d\n", philo->id, philo->right_fork->id);

		usleep(arg->time_to_eat * 1000);
		printf("%d is eating\n", philo->id);
		philo->nbr_meals++;
		printf("nb_meals = %d\n", philo->nbr_meals);

		printf("Philo %d left fork nb %d\n", philo->id, philo->left_fork->id);
		pthread_mutex_unlock(&philo->left_fork->fork);
		printf("Philo %d left fork nb %d\n", philo->id, philo->right_fork->id);
		pthread_mutex_unlock(&philo->right_fork->fork);

		usleep(arg->time_to_sleep * 1000);
		printf("%d is sleeping\n", philo->id);
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_arg	*arg;
	
	if (argc == 1)
		ft_exit("[number of philo] [time_to_die] [time_to_eat] [time_to_sleep] [must_eat]");
	arg = malloc(sizeof(t_arg));
	if (!arg)
		ft_exit("Memory allocation failed");
	if (!parse_arg(argc, argv, &arg))
	{
		free(arg);
		ft_exit("Invalid arguments");
	}
	init_philos_and_forks(arg);
	int i = 0;
	while (i < arg->nb_philo)
	{
		arg->philos[i].arg = arg;
		if (pthread_create(&arg->philos[i].philo, NULL, philo_routine, (void *)&arg->philos[i]) != 0)
			ft_exit("Failed to create thread");
		i++;
	}
	i = 0;
	while (i < arg->nb_philo)
	{
		pthread_join(arg->philos[i].philo, NULL);
		i++;
	}
	free(arg);
	return (0);
}