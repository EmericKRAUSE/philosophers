/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekrause <emeric.yukii@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 14:20:17 by ekrause           #+#    #+#             */
/*   Updated: 2024/11/12 17:26:13 by ekrause          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"
#include <string.h>
#include <sys/time.h>

void	ft_exit(char *error)
{
	printf(RED "%s\n" END, error);
	exit(1);
}

long	get_time()
{
	struct timeval time;
	
	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
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
		(*arg)->start = get_time();
		(*arg)->flag = 0;
		pthread_mutex_init(&(*arg)->end_mutex, NULL);
		pthread_mutex_init(&(*arg)->write_mutex, NULL);
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
		arg->philos[i].last_meal = arg->start;
		arg->philos[i].nbr_meals = 0;
		arg->philos[i].left_fork = &arg->forks[i];
		arg->philos[i].right_fork = &arg->forks[(i + 1) % arg->nb_philo];
		pthread_mutex_init(&arg->forks[i].fork, NULL);
		i++;
	}
}

void	write_status(t_philo *philo, char *status)
{
	if (philo->arg->flag)
		return;
	pthread_mutex_lock(&philo->arg->write_mutex);
	printf("[%ld] %ld %s\n", get_time() - philo->arg->start, philo->id, status);
	pthread_mutex_unlock(&philo->arg->write_mutex);
}

int	is_finished(t_arg *arg)
{
	int	i;

	i = 0;
	while (i < arg->nb_philo)
	{
		if (arg->philos[i].nbr_meals < arg->must_eat)
			return (0);
		i++;
	}
	return (1);
}

void	*monitor(void *data)
{
	t_arg *arg = (t_arg *)data;
	int i;
	long time_since_last_meal;

	while (!arg->flag)
	{
		i = 0;
		while (i < arg->nb_philo)
		{
			time_since_last_meal = get_time() - arg->philos[i].last_meal;
			if (time_since_last_meal > arg->time_to_die)
			{
				write_status(&arg->philos[i], "died");
				pthread_mutex_lock(&arg->end_mutex);
				arg->flag = 1;
				pthread_mutex_unlock(&arg->end_mutex);
			}
			if (is_finished(arg) && arg->must_eat != -1)
			{
				pthread_mutex_lock(&arg->end_mutex);
				arg->flag = 1;
				pthread_mutex_unlock(&arg->end_mutex);
			}
			i++;
		}
	}
	return (NULL);
}

void	*philo_routine(void *data)
{
	t_philo *philo = (t_philo *)data;
	t_arg	*arg = philo->arg;
	
	if (arg->nb_philo == 1)
	{
		write_status(philo, "is thinking");
		usleep(arg->time_to_die * 1000);
		write_status(philo, "died");
		return (NULL);
	}
	while (!arg->flag)
	{
		write_status(philo, "is thinking");

		if (philo->id % 2 == 0)
		{
			pthread_mutex_lock(&philo->left_fork->fork);
			write_status(philo, "has taken a fork");
			pthread_mutex_lock(&philo->right_fork->fork);
			write_status(philo, "has taken a fork");
		}
		else
		{
			pthread_mutex_lock(&philo->right_fork->fork);
			write_status(philo, "has taken a fork");
			pthread_mutex_lock(&philo->left_fork->fork);
			write_status(philo, "has taken a fork");
		}

		philo->last_meal = get_time();
		philo->nbr_meals++;
		write_status(philo, "is eating");
		usleep(arg->time_to_eat * 1000);
		pthread_mutex_unlock(&philo->right_fork->fork);
		pthread_mutex_unlock(&philo->left_fork->fork);
		write_status(philo, "is sleeping");
		usleep(arg->time_to_sleep * 1000);
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_arg	*arg;
	int		i;
	pthread_t monitor_var;

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
	i = -1;
	if (pthread_create(&monitor_var, NULL, monitor, (void *)arg) != 0)
		ft_exit("Failed to create thread");
	while (++i < arg->nb_philo)
	{
		arg->philos[i].arg = arg;
		if (pthread_create(&arg->philos[i].philo, NULL, philo_routine, (void *)&arg->philos[i]) != 0)
			ft_exit("Failed to create thread");
	}
	i = -1;
	while (++i < arg->nb_philo)
		pthread_join(arg->philos[i].philo, NULL);
	free(arg);
	return (0);
}